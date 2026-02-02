#ifndef LCDDISPLAYCONTROLLER_H
#define LCDDISPLAYCONTROLLER_H

#include "DisplayItem.h"
#include "DisplayConfig.h"
#include "IRenderer.h"
#include <vector>
#include <memory>
#include <stdexcept>

/**
 * Generic LCD Display Controller using templates.
 * 
 * This class manages navigation and interaction with a list of key-value display items.
 * Widths are extracted from the DisplayItem type at compile-time.
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
    size_t navigatorRow;
    bool isSelected;

    /**
     * Format a single row for display.
     */
    std::string formatRow(size_t rowIndex) const
    {
        std::string line;
        
        // Add navigator character (only on selected row)
        line += (rowIndex == navigatorRow) ? config.navigatorChar : ' ';
        
        // Add key and value with separator
        if (rowIndex < items.size())
        {
            line += items[rowIndex].getFormattedKey();
            line += config.separatorChar;
            line += items[rowIndex].getFormattedValue();
        }
        else
        {
            // Fill empty rows with spaces (no item at this row)
            // Account for navigator (1) + separator (1) already counted
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
     * Validate row index.
     */
    void validateRowIndex(size_t rowIndex) const
    {
        if (rowIndex >= items.size())
        {
            throw std::out_of_range("Row index out of range");
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
      renderer(renderer), navigatorRow(0), isSelected(false)
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
     * Navigate to previous item.
     */
    void navigateUp()
    {
        if (navigatorRow > 0)
        {
            --navigatorRow;
        }
        render();
    }

    /**
     * Navigate to next item.
     */
    void navigateDown()
    {
        if (navigatorRow < config.rows - 1 && navigatorRow < items.size() - 1)
        {
            ++navigatorRow;
        }
        render();
    }

    /**
     * Mark current item as selected.
     */
    void selectItem()
    {
        isSelected = true;
        render();
    }

    /**
     * Mark current item as deselected.
     */
    void deselectItem()
    {
        isSelected = false;
        render();
    }

    /**
     * Set the value of the currently selected item.
     */
    template<typename TValue>
    void setCurrentValue(const TValue& newValue)
    {
        validateRowIndex(navigatorRow);
        items[navigatorRow].setValue(newValue);
        render();
    }

    /**
     * Get the value of the currently selected item.
     */
    auto getCurrentValue() const
    {
        validateRowIndex(navigatorRow);
        return items[navigatorRow].getValue();
    }

    /**
     * Get the key of the currently selected item.
     */
    auto getCurrentKey() const
    {
        validateRowIndex(navigatorRow);
        return items[navigatorRow].getKey();
    }

    /**
     * Get current navigator position.
     */
    size_t getNavigatorRow() const
    {
        return navigatorRow;
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
