#ifndef LCDDISPLAYCONTROLLER_H
#define LCDDISPLAYCONTROLLER_H

#include "DisplayItem.h"
#include "DisplayConfig.h"
#include "IRenderer.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>

/**
 * Generic LCD Display Controller using templates with scrolling support.
 * 
 * This class manages navigation and interaction with a list of key-value display items.
 * Supports scrolling through items when there are more items than visible rows.
 * Widths are extracted from the DisplayItem type at compile-time.
 * 
 * It follows SOLID principles:
 * - Single Responsibility: Manages display state and navigation logic
 * - Open/Closed: Extendable through templates and renderer injection
 * - Dependency Inversion: Depends on IRenderer abstraction, not concrete implementation
 * 
 * @tparam TDisplayItem The DisplayItem type (includes key type, value type, and widths)
 */
template<typename TDisplayItem>
class LCDDisplayController
{
private:
    DisplayConfig config;
    std::vector<TDisplayItem> items;
    std::shared_ptr<IRenderer> renderer;
    size_t selectedItemIndex;   // Index into items vector (0 to items.size()-1)
    size_t windowStartIndex;    // Index of first visible item in the window
    bool isSelected;

    /**
     * Get the row position of the selected item within the visible window.
     * @return Row index (0 to config.rows-1) where the cursor appears
     */
    size_t getNavigatorRowInWindow() const
    {
        return selectedItemIndex - windowStartIndex;
    }

    /**
     * Format a single row for display.
     * @param rowIndex The row index within the visible window (0 to config.rows-1)
     */
    std::string formatRow(size_t rowIndex) const
    {
        std::string line;
        size_t itemIndex = windowStartIndex + rowIndex;
        
        // Add navigator character (only on selected row)
        line += (rowIndex == getNavigatorRowInWindow()) ? config.navigatorChar : ' ';
        
        // Add key and value with separator
        if (itemIndex < items.size())
        {
            line += items[itemIndex].getFormattedKey();
            line += config.separatorChar;
            line += items[itemIndex].getFormattedValue();
        }
        else
        {
            // Fill empty rows with spaces (no item at this row)
            size_t remainingSpace = (config.columns > 1) ? config.columns - 1 : 0;
            line += std::string(remainingSpace, ' ');
        }
        
        // Ensure exact column width (pad or truncate)
        if (line.length() < config.columns)
        {
            line += std::string(config.columns - line.length(), ' ');
        }
        else if (line.length() > config.columns)
        {
            line = line.substr(0, config.columns);
        }
        
        return line;
    }

    /**
     * Validate item index.
     */
    void validateItemIndex(size_t itemIndex) const
    {
        if (itemIndex >= items.size())
        {
            throw std::out_of_range("Item index out of range");
        }
    }

    /**
     * Adjust the visible window to ensure the selected item is visible.
     * This implements the scrolling behavior.
     */
    void adjustWindow()
    {
        if (items.empty())
        {
            windowStartIndex = 0;
            return;
        }

        // If selected item is above the window, scroll up
        if (selectedItemIndex < windowStartIndex)
        {
            windowStartIndex = selectedItemIndex;
        }
        // If selected item is below the window, scroll down
        else if (selectedItemIndex >= windowStartIndex + config.rows)
        {
            windowStartIndex = selectedItemIndex - config.rows + 1;
        }
    }

public:
/**
 * Constructor with dependency injection.
 * Includes compile-time validation that item widths fit within display columns.
 * 
 * @param items Vector of DisplayItems to manage
 * @param renderer Rendering implementation
 * @param config Display configuration
 */
LCDDisplayController(
    std::vector<TDisplayItem> items,
    std::shared_ptr<IRenderer> renderer,
    const DisplayConfig& config = DisplayConfig())
    : config(config), items(std::move(items)), 
      renderer(renderer), selectedItemIndex(0), windowStartIndex(0), isSelected(false)
{
    // Compile-time validation: Ensure item widths fit within display columns
    // Format: [navigator(1)] + [key(KeyWidth)] + [separator(1)] + [value(ValueWidth)]
    constexpr size_t navigatorWidth = 1;
    constexpr size_t separatorWidth = 1;
    constexpr size_t keyWidth = TDisplayItem::getKeyWidth();
    constexpr size_t valueWidth = TDisplayItem::getValueWidth();
    constexpr size_t requiredWidth = navigatorWidth + keyWidth + separatorWidth + valueWidth;
        
    static_assert(requiredWidth <= 256, 
        "Total required width exceeds maximum reasonable display width (256 columns). "
        "Check your DisplayItem template parameters.");
        
    // Runtime validation: Check config matches item requirements
    if (!renderer)
    {
        throw std::invalid_argument("Renderer cannot be null");
    }
        
    if (config.columns < requiredWidth)
    {
        throw std::invalid_argument(
            "DisplayConfig columns (" + std::to_string(config.columns) + 
            ") is too small for DisplayItem width requirements (" + 
            std::to_string(requiredWidth) + " = 1 navigator + " +
            std::to_string(keyWidth) + " key + 1 separator + " +
            std::to_string(valueWidth) + " value)");
    }
}

    /**
     * Render the current display state.
     */
    void render()
    {
        std::vector<std::string> lines;
        lines.reserve(config.rows);
        
        for (size_t i = 0; i < config.rows; ++i)
        {
            lines.push_back(formatRow(i));
        }
        
        renderer->render(lines, config.columns);
    }

    /**
     * Navigate to previous item (scrolls if necessary).
     * @return true if navigation occurred, false if already at top
     */
    bool navigateUp()
    {
        if (selectedItemIndex > 0)
        {
            --selectedItemIndex;
            adjustWindow();
            render();
            return true;
        }
        return false;
    }

    /**
     * Navigate to next item (scrolls if necessary).
     * @return true if navigation occurred, false if already at bottom
     */
    bool navigateDown()
    {
        if (!items.empty() && selectedItemIndex < items.size() - 1)
        {
            ++selectedItemIndex;
            adjustWindow();
            render();
            return true;
        }
        return false;
    }

    /**
     * Mark current item as selected.
     * @return true if state changed, false if already selected
     */
    bool selectItem()
    {
        if (!isSelected)
        {
            isSelected = true;
            render();
            return true;
        }
        return false;
    }

    /**
     * Mark current item as deselected.
     * @return true if state changed, false if already deselected
     */
    bool deselectItem()
    {
        if (isSelected)
        {
            isSelected = false;
            render();
            return true;
        }
        return false;
    }

    /**
     * Set the value of the currently selected item.
     */
    template<typename TValue>
    void setCurrentValue(const TValue& newValue)
    {
        validateItemIndex(selectedItemIndex);
        items[selectedItemIndex].setValue(newValue);
        render();
    }

    /**
     * Get the value of the currently selected item.
     */
    auto getCurrentValue() const
    {
        validateItemIndex(selectedItemIndex);
        return items[selectedItemIndex].getValue();
    }

    /**
     * Get the key of the currently selected item.
     */
    auto getCurrentKey() const
    {
        validateItemIndex(selectedItemIndex);
        return items[selectedItemIndex].getKey();
    }

    /**
     * Get current selected item index (0-based, in the full items list).
     */
    size_t getSelectedItemIndex() const
    {
        return selectedItemIndex;
    }

    /**
     * Get the index of the first visible item in the window.
     */
    size_t getWindowStartIndex() const
    {
        return windowStartIndex;
    }

    /**
     * Get current navigator position within the visible window.
     * @return Row index (0 to config.rows-1) where the cursor appears
     */
    size_t getNavigatorRow() const
    {
        return getNavigatorRowInWindow();
    }

    /**
     * Get total number of items.
     */
    size_t getItemCount() const
    {
        return items.size();
    }

    /**
     * Check if scrolling is possible (more items than visible rows).
     */
    bool canScroll() const
    {
        return items.size() > config.rows;
    }

    /**
     * Check if an item is currently selected.
     */
    bool getIsSelected() const
    {
        return isSelected;
    }

    /**
     * Get reference to items for advanced manipulation.
     */
    std::vector<TDisplayItem>& getItems()
    {
        return items;
    }

    const std::vector<TDisplayItem>& getItems() const
    {
        return items;
    }
};

#endif // LCDDISPLAYCONTROLLER_H
