#include <gtest/gtest.h>
#include "LCDDisplayController.h"
#include "DisplayItem.h"
#include "DisplayConfig.h"
#include "MockRenderer.h"
#include <memory>
#include <vector>

// Use a simple display item type for testing
using TestDisplayItem = DisplayItem<std::string, int, 10, 4>;

class DisplayControllerTests : public ::testing::Test
{
protected:
    std::shared_ptr<MockRenderer> mockRenderer;
    DisplayConfig config;
    
    void SetUp() override
    {
        mockRenderer = std::make_shared<MockRenderer>();
        config = DisplayConfig(2, 16, '>', ':');
    }

    std::vector<TestDisplayItem> createItems(int count)
    {
        std::vector<TestDisplayItem> items;
        for (int i = 0; i < count; ++i)
        {
            items.emplace_back("Item" + std::to_string(i), i * 10);
        }
        return items;
    }
};

TEST_F(DisplayControllerTests, ConstructorWithValidParametersSucceeds)
{
    auto items = createItems(3);
    
    EXPECT_NO_THROW({
        LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    });
}

TEST_F(DisplayControllerTests, ConstructorWithNullRendererThrows)
{
    auto items = createItems(3);
    
    EXPECT_THROW({
        LCDDisplayController<TestDisplayItem> controller(items, nullptr, config);
    }, std::invalid_argument);
}

TEST_F(DisplayControllerTests, ConstructorWithTooSmallColumnWidthThrows)
{
    auto items = createItems(3);
    DisplayConfig smallConfig(2, 10, '>', ':'); // Too small for 10+4+1+1 = 16 (TestDisplayItem)
    
    EXPECT_THROW({
        LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, smallConfig);
    }, std::invalid_argument);
}

TEST_F(DisplayControllerTests, RenderCallsRendererWithCorrectLineCount)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.render();
    
    EXPECT_EQ(mockRenderer->getLineCount(), config.rows);
    EXPECT_EQ(mockRenderer->getLineCount(), 2);
    EXPECT_EQ(config.rows, 2);
}

TEST_F(DisplayControllerTests, RenderFormatsLinesWithCorrectWidth)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.render();
    
    for (size_t i = 0; i < mockRenderer->getLineCount(); ++i)
    {
        EXPECT_EQ(mockRenderer->getLine(i).length(), config.columns);
        EXPECT_EQ(mockRenderer->getLine(i).length(), 16);
        EXPECT_EQ(config.columns, 16);
    }
}

TEST_F(DisplayControllerTests, InitialSelectedItemIndexIsZero)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
}

TEST_F(DisplayControllerTests, NavigatorCharAppearsOnSelectedRow)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.render();
    
    // First character of first line should be the navigator
    EXPECT_EQ(mockRenderer->getLine(0)[0], '>');
    // First character of second line should be space
    EXPECT_EQ(mockRenderer->getLine(1)[0], ' ');
}

TEST_F(DisplayControllerTests, NavigateDownMovesSelection)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    bool changed = controller.navigateDown();
    
    EXPECT_TRUE(changed);
    EXPECT_EQ(controller.getSelectedItemIndex(), 1);
}

TEST_F(DisplayControllerTests, NavigateUpMovesSelection)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.navigateDown(); // Move to index 1
    bool changed = controller.navigateUp();   // Move back to index 0
    
    EXPECT_TRUE(changed);
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
}

TEST_F(DisplayControllerTests, NavigateUpAtTopReturnsFalse)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    bool changed = controller.navigateUp(); // Already at top
    
    EXPECT_FALSE(changed);
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
}

TEST_F(DisplayControllerTests, NavigateUpAtTopDoesNotRender)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    mockRenderer->reset();
    
    controller.navigateUp(); // Already at top, should not render
    
    EXPECT_EQ(mockRenderer->renderCallCount, 0);
}

TEST_F(DisplayControllerTests, GetCurrentValueReturnsSelectedItemValue)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getCurrentValue(), 0); // First item has value 0
    
    controller.navigateDown();
    EXPECT_EQ(controller.getCurrentValue(), 10); // Second item has value 10
}

TEST_F(DisplayControllerTests, GetCurrentKeyReturnsSelectedItemKey)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getCurrentKey(), "Item0");
    
    controller.navigateDown();
    EXPECT_EQ(controller.getCurrentKey(), "Item1");
}

TEST_F(DisplayControllerTests, SetCurrentValueUpdatesSelectedItem)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.setCurrentValue(999);
    
    EXPECT_EQ(controller.getCurrentValue(), 999);
}

TEST_F(DisplayControllerTests, SelectItemSetsIsSelectedTrue)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.selectItem();
    
    EXPECT_TRUE(controller.getIsSelected());
}

TEST_F(DisplayControllerTests, SelectItemReturnsFalseWhenAlreadySelected)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.selectItem();
    mockRenderer->reset();
    bool changed = controller.selectItem();
    
    EXPECT_FALSE(changed);
    EXPECT_EQ(mockRenderer->renderCallCount, 0);
}

TEST_F(DisplayControllerTests, DeselectItemSetsIsSelectedFalse)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    controller.selectItem();
    controller.deselectItem();
    
    EXPECT_FALSE(controller.getIsSelected());
}

TEST_F(DisplayControllerTests, DeselectItemReturnsFalseWhenAlreadyDeselected)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    mockRenderer->reset();
    
    bool changed = controller.deselectItem();
    
    EXPECT_FALSE(changed);
    EXPECT_EQ(mockRenderer->renderCallCount, 0);
}

TEST_F(DisplayControllerTests, GetItemsReturnsAllItems)
{
    auto items = createItems(5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getItems().size(), 5);
}

TEST_F(DisplayControllerTests, GetItemCountReturnsCorrectCount)
{
    auto items = createItems(7);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getItemCount(), 7);
}

TEST_F(DisplayControllerTests, EmptyItemsVectorIsHandled)
{
    std::vector<TestDisplayItem> empty;
    LCDDisplayController<TestDisplayItem> controller(empty, mockRenderer, config);
    
    EXPECT_EQ(controller.getItemCount(), 0);
    
    // Render should work without crashing
    EXPECT_NO_THROW(controller.render());
}
