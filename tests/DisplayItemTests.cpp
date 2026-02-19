#include <gtest/gtest.h>
#include "DisplayItem.h"
#include <string>

// Use a simple display item type for testing
using TestDisplayItem = DisplayItem<std::string, int, 8, 4>;

class DisplayItemTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};

TEST_F(DisplayItemTests, ConstructorSetsKeyAndValue)
{
    TestDisplayItem item("TestKey", 42);
    
    EXPECT_EQ(item.getKey(), "TestKey");
    EXPECT_EQ(item.getValue(), 42);
}

TEST_F(DisplayItemTests, DefaultConstructorCreatesEmptyItem)
{
    TestDisplayItem item;
    
    EXPECT_EQ(item.getKey(), "");
    EXPECT_EQ(item.getValue(), 0);
}

TEST_F(DisplayItemTests, SetValueUpdatesValue)
{
    TestDisplayItem item("TestKey", 42);
    item.setValue(100);
    
    EXPECT_EQ(item.getValue(), 100);
}

TEST_F(DisplayItemTests, SetKeyUpdatesKey)
{
    TestDisplayItem item("OldKey", 42);
    item.setKey("NewKey");
    
    EXPECT_EQ(item.getKey(), "NewKey");
}

TEST_F(DisplayItemTests, GetFormattedKeyPadsShortKeys)
{
    TestDisplayItem item("Hi", 42);
    std::string formatted = item.getFormattedKey();
    
    // Key width is 8, "Hi" should be padded to 8 characters
    EXPECT_EQ(formatted.length(), 8);
    EXPECT_EQ(formatted, "Hi      ");
}

TEST_F(DisplayItemTests, GetFormattedKeyTruncatesLongKeys)
{
    TestDisplayItem item("VeryLongKeyName", 42);
    std::string formatted = item.getFormattedKey();
    
    // Key width is 8, should be truncated
    EXPECT_EQ(formatted.length(), 8);
    EXPECT_EQ(formatted, "VeryLong");
}

TEST_F(DisplayItemTests, GetFormattedValuePadsShortValues)
{
    TestDisplayItem item("Key", 5);
    std::string formatted = item.getFormattedValue();
    
    // Value width is 4, "5" should be padded
    EXPECT_EQ(formatted.length(), 4);
}

TEST_F(DisplayItemTests, GetKeyWidthReturnsTemplateParameter)
{
    EXPECT_EQ(TestDisplayItem::getKeyWidth(), 8);
}

TEST_F(DisplayItemTests, GetValueWidthReturnsTemplateParameter)
{
    EXPECT_EQ(TestDisplayItem::getValueWidth(), 4);
}

TEST_F(DisplayItemTests, GetTotalWidthReturnsSumOfWidths)
{
    EXPECT_EQ(TestDisplayItem::getTotalWidth(), 12);
}

// Test with different types
using StringValueItem = DisplayItem<std::string, std::string, 6, 10>;

TEST_F(DisplayItemTests, WorksWithStringValues)
{
    StringValueItem item("Name", "TestValue");
    
    EXPECT_EQ(item.getKey(), "Name");
    EXPECT_EQ(item.getValue(), "TestValue");
}

// Test with uint8_t (common for inventory counts)
using InventoryItem = DisplayItem<std::string, uint8_t, 11, 3>;

TEST_F(DisplayItemTests, WorksWithUint8Values)
{
    InventoryItem item("Sword", static_cast<uint8_t>(99));
    
    EXPECT_EQ(item.getValue(), 99);
    // uint8_t should be displayed as a number, not a character
    std::string formatted = item.getFormattedValue();
    EXPECT_EQ(formatted, "99 ");
}

// Test edge cases
TEST_F(DisplayItemTests, HandlesEmptyKey)
{
    TestDisplayItem item("", 42);
    std::string formatted = item.getFormattedKey();
    
    EXPECT_EQ(formatted.length(), 8);
    EXPECT_EQ(formatted, "        ");
}

TEST_F(DisplayItemTests, HandlesZeroValue)
{
    TestDisplayItem item("Key", 0);
    EXPECT_EQ(item.getValue(), 0);
}

TEST_F(DisplayItemTests, HandlesNegativeValue)
{
    TestDisplayItem item("Key", -42);
    EXPECT_EQ(item.getValue(), -42);
}
