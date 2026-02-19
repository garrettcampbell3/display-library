#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "LCDInventoryController.h"
#include "DisplayItem.h"
#include "DisplayConfig.h"
#include "ConsoleRenderer.h"
#include "ConsoleInputListener.h"

int main(int, char**)
{
    // We want to have a display that mimics the LCD screen. it's 2 rows by 16 columns.
    std::cout << "Two Row Display Library Initialized." << std::endl;
    std::cout << "Using new generic template-based architecture." << std::endl;

    // Create display items using compile-time width enforcement
    // Using InventoryDisplayItem alias: DisplayItem<std::string, uint8_t, 11, 3>
    // All items in this vector are GUARANTEED to have 11-char keys and 3-char values
    const uint8_t KEY_WIDTH = 11;
    const uint8_t VALUE_WIDTH = 3;
    using InventoryDisplayItem = DisplayItem<std::string, uint8_t, KEY_WIDTH, VALUE_WIDTH>;
    std::vector<InventoryDisplayItem> items;

    for (int i = 0; i < 10; i++) {
        items.emplace_back("Item" + std::to_string(i + 1), static_cast<uint8_t>(0));
    }

    // Create console renderer
    auto renderer = std::make_shared<ConsoleRenderer>();

    // Create display configuration
    // Widths are now in the DisplayItem type, not the config!
    // Config only controls: rows, columns, navigator char, separator char
    DisplayConfig config(2, 16, '>', ':');

    // Create inventory controller using the new compile-time enforced design
    // The type system guarantees all items have identical widths
    // Type alias for common inventory use case: string keys, uint8_t values, 11x3 display
    LCDInventoryController<InventoryDisplayItem> controller(items, renderer, config);

    // Set up input listener
    ConsoleInputListener inputListener;
    inputListener.startListening();
    inputListener.printHelp();

    controller.render();

    while (true) {
        NavigationCommand command = inputListener.waitForCommand();

        switch (command)
        {
        case NavigationCommand::Up:
            controller.navigateUp();
            break;
        case NavigationCommand::Down:
            controller.navigateDown();
            break;
        case NavigationCommand::Select:
            controller.selectItem();
            break;
        case NavigationCommand::Deselect:
            controller.deselectItem();
            break;
        case NavigationCommand::Increment:
            controller.incrementValue();
            break;
        case NavigationCommand::Decrement:
            controller.decrementValue();
            break;
        case NavigationCommand::None:
            // No action
            break;
        }

        // Small delay to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}
