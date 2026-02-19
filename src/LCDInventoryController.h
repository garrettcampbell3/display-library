#ifndef LCDINVENTORYCONTROLLER_H
#define LCDINVENTORYCONTROLLER_H

#include "LCDDisplayController.h"
#include "DisplayItem.h"
#include "IInventoryController.h"
#include <type_traits>

/**
 * Specialized LCD controller for inventory management with numeric values.
 * Extends LCDDisplayController with increment/decrement operations.
 * 
 * @tparam TDisplayItem The DisplayItem type (must have arithmetic value type)
 */
template<typename TDisplayItem>
class LCDInventoryController : public IInventoryController
{
private:
    LCDDisplayController<TDisplayItem> displayController;

public:
    /**
     * Constructor with dependency injection.
     */
    LCDInventoryController(
        std::vector<TDisplayItem> items,
        std::shared_ptr<IRenderer> renderer,
        const DisplayConfig& config = DisplayConfig())
        : displayController(std::move(items), renderer, config)
    {
    }

    void navigateUp() override
    {
        displayController.navigateUp();
    }

    void navigateDown() override
    {
        displayController.navigateDown();
    }

    void selectItem() override
    {
        displayController.selectItem();
    }

    void deselectItem() override
    {
        displayController.deselectItem();
    }

    void incrementValue() override
    {
        auto currentValue = displayController.getCurrentValue();
        displayController.setCurrentValue(currentValue + 1);
    }

    void decrementValue() override
    {
        auto currentValue = displayController.getCurrentValue();
        displayController.setCurrentValue(currentValue - 1);
    }

    /**
     * Render the display.
     */
    void render()
    {
        displayController.render();
    }

    /**
     * Get access to the underlying display controller for advanced operations.
     */
    LCDDisplayController<TDisplayItem>& getDisplayController()
    {
        return displayController;
    }

    const LCDDisplayController<TDisplayItem>& getDisplayController() const
    {
        return displayController;
    }
};

#endif // LCDINVENTORYCONTROLLER_H
