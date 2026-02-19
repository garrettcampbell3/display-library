#include <gtest/gtest.h>
#include "LCDDisplayController.h"
#include "DisplayItem.h"
#include "DisplayConfig.h"
#include "MockRenderer.h"
#include <memory>
#include <vector>

// Use a simple display item type for testing
using TestDisplayItem = DisplayItem<std::string, int, 10, 4>;

// ============================================================================
// Parameterized test fixture for testing scrolling with different row counts
// ============================================================================

class ScrollingTestsParameterized : public ::testing::TestWithParam<size_t>
{
protected:
    std::shared_ptr<MockRenderer> mockRenderer;
    DisplayConfig config;
    size_t rowCount;
    
    void SetUp() override
    {
        mockRenderer = std::make_shared<MockRenderer>();
        rowCount = GetParam();
        config = DisplayConfig(rowCount, 16, '>', ':');
    }

    std::vector<TestDisplayItem> createItems(int count)
    {
        std::vector<TestDisplayItem> items;
        for (int i = 0; i < count; ++i)
        {
            items.emplace_back("Item" + std::to_string(i), i);
        }
        return items;
    }
};

// Test with 2, 4, and 6 row displays
INSTANTIATE_TEST_SUITE_P(
    MultiRowDisplay,
    ScrollingTestsParameterized,
    ::testing::Values(2, 4, 6),
    [](const ::testing::TestParamInfo<size_t>& info) {
        return std::to_string(info.param) + "Rows";
    }
);

// ============================================================================
// Basic Scrolling Support Tests (Parameterized)
// ============================================================================

TEST_P(ScrollingTestsParameterized, CanScrollReturnsFalseWhenItemsEqualRows)
{
    auto items = createItems(static_cast<int>(rowCount));
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_FALSE(controller.canScroll());
}

TEST_P(ScrollingTestsParameterized, CanScrollReturnsTrueWhenItemsExceedRows)
{
    auto items = createItems(static_cast<int>(rowCount) + 3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_TRUE(controller.canScroll());
}

TEST_P(ScrollingTestsParameterized, InitialWindowStartIndexIsZero)
{
    auto items = createItems(static_cast<int>(rowCount) + 5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_EQ(controller.getWindowStartIndex(), 0);
}

// ============================================================================
// Navigate Down Scrolling Tests (Parameterized)
// ============================================================================

TEST_P(ScrollingTestsParameterized, NavigateDownWithinWindowDoesNotScroll)
{
    auto items = createItems(static_cast<int>(rowCount) + 5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Move through all visible rows (should not scroll)
    for (size_t i = 0; i < rowCount - 1; ++i)
    {
        controller.navigateDown();
    }
    
    EXPECT_EQ(controller.getSelectedItemIndex(), rowCount - 1);
    EXPECT_EQ(controller.getWindowStartIndex(), 0); // Window hasn't moved
}

TEST_P(ScrollingTestsParameterized, NavigateDownPastWindowScrollsDown)
{
    auto items = createItems(static_cast<int>(rowCount) + 5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate past the visible window
    for (size_t i = 0; i < rowCount; ++i)
    {
        controller.navigateDown();
    }
    
    EXPECT_EQ(controller.getSelectedItemIndex(), rowCount);
    EXPECT_EQ(controller.getWindowStartIndex(), 1); // Window scrolled by 1
}

TEST_P(ScrollingTestsParameterized, NavigateDownToLastItemScrollsCorrectly)
{
    int itemCount = static_cast<int>(rowCount) + 5;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate to last item
    for (int i = 0; i < itemCount - 1; ++i)
    {
        controller.navigateDown();
    }
    
    EXPECT_EQ(controller.getSelectedItemIndex(), static_cast<size_t>(itemCount - 1));
    // Window should show the last 'rowCount' items
    EXPECT_EQ(controller.getWindowStartIndex(), static_cast<size_t>(itemCount) - rowCount);
}

TEST_P(ScrollingTestsParameterized, NavigateDownAtBottomReturnsFalse)
{
    int itemCount = static_cast<int>(rowCount) + 3;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate to last item
    for (int i = 0; i < itemCount - 1; ++i)
    {
        controller.navigateDown();
    }
    
    // Try to navigate past the end
    bool changed = controller.navigateDown();
    
    EXPECT_FALSE(changed);
    EXPECT_EQ(controller.getSelectedItemIndex(), static_cast<size_t>(itemCount - 1));
}

// ============================================================================
// Navigate Up Scrolling Tests (Parameterized)
// ============================================================================

TEST_P(ScrollingTestsParameterized, NavigateUpWithinWindowDoesNotScroll)
{
    auto items = createItems(static_cast<int>(rowCount) + 5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Move down then back up within window
    controller.navigateDown();
    controller.navigateUp();
    
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
    EXPECT_EQ(controller.getWindowStartIndex(), 0);
}

TEST_P(ScrollingTestsParameterized, NavigateUpPastWindowScrollsUp)
{
    int itemCount = static_cast<int>(rowCount) + 5;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate past the window
    for (size_t i = 0; i < rowCount + 2; ++i)
    {
        controller.navigateDown();
    }
    size_t expectedWindow = 3; // After navigating rowCount+2 times from 0
    EXPECT_EQ(controller.getWindowStartIndex(), expectedWindow);
    
    // Navigate up past the visible area
    for (size_t i = 0; i < rowCount; ++i)
    {
        controller.navigateUp();
    }
    
    // Should have scrolled up
    EXPECT_LT(controller.getWindowStartIndex(), expectedWindow);
}

TEST_P(ScrollingTestsParameterized, NavigateUpToFirstItemScrollsCorrectly)
{
    int itemCount = static_cast<int>(rowCount) + 5;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate to last item
    for (int i = 0; i < itemCount - 1; ++i)
    {
        controller.navigateDown();
    }
    
    // Navigate back to first item
    for (int i = 0; i < itemCount - 1; ++i)
    {
        controller.navigateUp();
    }
    
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
    EXPECT_EQ(controller.getWindowStartIndex(), 0);
}

// ============================================================================
// Navigator Row Tests (Parameterized)
// ============================================================================

TEST_P(ScrollingTestsParameterized, NavigatorRowInWindowIsCorrectDuringScrollDown)
{
    int itemCount = static_cast<int>(rowCount) + 5;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // At start, navigator is on row 0 of window
    EXPECT_EQ(controller.getNavigatorRow(), 0);
    
    // Navigate through all visible rows
    for (size_t i = 1; i < rowCount; ++i)
    {
        controller.navigateDown();
        EXPECT_EQ(controller.getNavigatorRow(), i);
    }
    
    // After scrolling, navigator should stay on last row of window
    controller.navigateDown();
    EXPECT_EQ(controller.getNavigatorRow(), rowCount - 1);
}

// ============================================================================
// Performance Tests (Parameterized)
// ============================================================================

TEST_P(ScrollingTestsParameterized, NavigateDownAtBottomDoesNotRender)
{
    int itemCount = static_cast<int>(rowCount) + 2;
    auto items = createItems(itemCount);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate to bottom
    for (int i = 0; i < itemCount - 1; ++i)
    {
        controller.navigateDown();
    }
    mockRenderer->reset();
    
    // Try to navigate past bottom
    controller.navigateDown();
    
    EXPECT_EQ(mockRenderer->renderCallCount, 0);
}

TEST_P(ScrollingTestsParameterized, NavigateUpAtTopDoesNotRender)
{
    auto items = createItems(static_cast<int>(rowCount) + 2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    mockRenderer->reset();
    
    // Try to navigate past top
    controller.navigateUp();
    
    EXPECT_EQ(mockRenderer->renderCallCount, 0);
}

// ============================================================================
// Non-parameterized fixture for 2-row specific tests
// ============================================================================

class ScrollingTests : public ::testing::Test
{
protected:
    std::shared_ptr<MockRenderer> mockRenderer;
    DisplayConfig config;
    
    void SetUp() override
    {
        mockRenderer = std::make_shared<MockRenderer>();
        // 2-row display for scrolling tests
        config = DisplayConfig(2, 16, '>', ':');
    }

    std::vector<TestDisplayItem> createItems(int count)
    {
        std::vector<TestDisplayItem> items;
        for (int i = 0; i < count; ++i)
        {
            items.emplace_back("Item" + std::to_string(i), i);
        }
        return items;
    }
};

// ============================================================================
// 2-Row Specific Tests (content verification that depends on exact layout)
// ============================================================================

TEST_F(ScrollingTests, CanScrollReturnsFalseWhenItemsLessThanRows)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_FALSE(controller.canScroll());
}

TEST_F(ScrollingTests, CanScrollReturnsTrueWhenItemsExceedRows)
{
    auto items = createItems(5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_TRUE(controller.canScroll());
}

// ============================================================================
// Render Content Verification Tests (2-Row Specific)
// ============================================================================

TEST_F(ScrollingTests, RenderShowsCorrectItemsAfterScrollDown_TwoRows)
{
    auto items = createItems(5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // Navigate to item 3 (window should show items 2-3)
    for (int i = 0; i < 3; ++i)
    {
        controller.navigateDown();
    }
    
    // Verify rendered content shows items 2 and 3
    std::string line0 = mockRenderer->getLine(0);
    std::string line1 = mockRenderer->getLine(1);
    
    // Item2 should be on first visible row (no navigator)
    EXPECT_EQ(line0[0], ' ');
    EXPECT_NE(line0.find("Item2"), std::string::npos);
    
    // Item3 should be on second visible row (with navigator)
    EXPECT_EQ(line1[0], '>');
    EXPECT_NE(line1.find("Item3"), std::string::npos);
}

TEST_F(ScrollingTests, NavigatorRowInWindowIsCorrect_TwoRows)
{
    auto items = createItems(5);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    // At start, navigator is on row 0 of window
    EXPECT_EQ(controller.getNavigatorRow(), 0);
    
    // Move to item 1, navigator is on row 1 of window
    controller.navigateDown();
    EXPECT_EQ(controller.getNavigatorRow(), 1);
    
    // Move to item 2, window scrolls, navigator stays on row 1 of window
    controller.navigateDown();
    EXPECT_EQ(controller.getNavigatorRow(), 1);
    
    // Move to item 3, window scrolls, navigator stays on row 1 of window
    controller.navigateDown();
    EXPECT_EQ(controller.getNavigatorRow(), 1);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ScrollingTests, SingleItemDoesNotScroll)
{
    auto items = createItems(1);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_FALSE(controller.canScroll());
    
    controller.navigateDown();
    EXPECT_EQ(controller.getSelectedItemIndex(), 0);
}

TEST_F(ScrollingTests, ExactlyTwoItemsInTwoRowDisplayDoesNotScroll)
{
    auto items = createItems(2);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_FALSE(controller.canScroll());
    
    controller.navigateDown();
    EXPECT_EQ(controller.getSelectedItemIndex(), 1);
    EXPECT_EQ(controller.getWindowStartIndex(), 0);
}

TEST_F(ScrollingTests, ThreeItemsInTwoRowDisplayCanScroll)
{
    auto items = createItems(3);
    LCDDisplayController<TestDisplayItem> controller(items, mockRenderer, config);
    
    EXPECT_TRUE(controller.canScroll());
    
    // Navigate to last item
    controller.navigateDown();
    controller.navigateDown();
    
    EXPECT_EQ(controller.getSelectedItemIndex(), 2);
    EXPECT_EQ(controller.getWindowStartIndex(), 1); // Shows items 1-2
}
