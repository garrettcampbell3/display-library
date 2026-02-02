# Generic LCD Display Controller Library

A well-architected, template-based C++ library for managing LCD displays with key-value navigation.

## Design Principles

This library follows SOLID principles and clean code practices:

- **Single Responsibility Principle**: Each class has one clear purpose
- **Open/Closed Principle**: Extensible through templates and interfaces without modification
- **Liskov Substitution Principle**: Interfaces are properly abstracted
- **Interface Segregation Principle**: Focused interfaces (IRenderer, IInventoryController)
- **Dependency Inversion Principle**: Depends on abstractions (IRenderer) not concretions

### Design Patterns Used

1. **Strategy Pattern**: `IRenderer` interface allows different rendering implementations
2. **Template Method Pattern**: Base display logic with specialized inventory operations
3. **Dependency Injection**: Controllers accept dependencies through constructors
4. **Composition over Inheritance**: Controllers compose functionality rather than deep hierarchies

## Architecture

### Core Components

#### 1. `DisplayItem<TKey, TValue, KeyWidth, ValueWidth>` (Template Class)
Generic key-value display item with **compile-time width enforcement**.

```cpp
template<typename TKey, typename TValue, size_t KeyWidth, size_t ValueWidth>
class DisplayItem;
```

**Features:**
- Automatic type-to-string conversion
- Type-safe key-value storage
- **Compile-time width enforcement** - widths are part of the type!
- Zero runtime overhead for width management

**Design Principle:**
Widths are **template parameters**, not stored in instances. This means:
- ✅ Items with different widths are **different types** at compile-time
- ✅ Impossible to mix items with different widths in the same vector
- ✅ Compiler catches width mismatches before your code even runs
- ✅ No runtime checks or storage overhead

**Example:**
```cpp
// These are DIFFERENT types:
DisplayItem<std::string, uint8_t, 11, 3> standardItem("Sword", 5);
DisplayItem<std::string, uint8_t, 10, 4> differentItem("Gold", 99);

// Cannot mix in same vector - COMPILE ERROR!
std::vector<DisplayItem<std::string, uint8_t, 11, 3>> items;
items.push_back(standardItem);   // ✅ OK
// items.push_back(differentItem);  // ❌ COMPILE ERROR: Wrong type!
```

**Type Aliases:**
```cpp
// Standard inventory format (11-char key, 3-char value)
using InventoryDisplayItem = DisplayItem<std::string, uint8_t, 11, 3>;
```

#### 2. `DisplayConfig` (Configuration Struct)
Encapsulates display parameters (rows, columns, characters).

```cpp
struct DisplayConfig {
    size_t rows;
    size_t columns;
    size_t keyWidth;      // Kept for backward compatibility
    size_t valueWidth;    // Kept for backward compatibility
    char navigatorChar;
    char separatorChar;
};
```

**Note:** With compile-time width enforcement, `keyWidth` and `valueWidth` in `DisplayConfig` are now **redundant** (widths come from the type). They're kept for backward compatibility but may be removed in future versions.

**Benefits:**
- Centralized configuration for display parameters
- Easy to pass and modify
- Default values provided (2 rows, 16 cols)

#### 3. `IRenderer` (Interface)
Abstract rendering interface following the Strategy pattern.

```cpp
class IRenderer {
    virtual void render(const std::vector<std::string>& lines, size_t columns) = 0;
    virtual void clear() = 0;
};
```

**Implementations:**
- `ConsoleRenderer`: Terminal-based rendering with ANSI codes
- Custom renderers can be easily created (LCD hardware, GUI, etc.)

**Benefits:**
- Separates display logic from business logic
- Testable (mock renderers for unit tests)
- Flexible (swap rendering without changing core logic)

#### 4. `LCDDisplayController<TDisplayItem>` (Template Class)
Core display management with navigation.

**Template Parameter:**
- `TDisplayItem` - The complete DisplayItem type (includes key type, value type, and widths)

**Features:**
- Generic key-value display
- Navigation (up/down)
- Selection state
- Rendering coordination
- Bounds checking and validation
- **Extracts widths from item type at compile-time**

**Example:**
```cpp
// Controller takes the complete item type
LCDDisplayController<InventoryDisplayItem> controller(items, renderer, config);
controller.navigateDown();
controller.setCurrentValue(42);
controller.render();
```

#### 5. `LCDInventoryController<TDisplayItem>` (Template Class)
Specialized controller for numeric inventory management.

**Features:**
- Extends base display controller
- Adds increment/decrement operations
- Works with any DisplayItem type that has arithmetic values
- Implements IInventoryController interface

**Example:**
```cpp
LCDInventoryController<InventoryDisplayItem> inventory(items, renderer);
inventory.incrementValue();  // Add 1 to current item
inventory.decrementValue();  // Subtract 1 from current item
```

## Type Aliases for Common Use Cases

```cpp
// Standard inventory: string keys, uint8_t values, 11-char key, 3-char value
using InventoryDisplayItem = DisplayItem<std::string, uint8_t, 11, 3>;
using InventoryController = LCDInventoryController<InventoryDisplayItem>;
```

Create your own:
```cpp
// Compact format
using CompactItem = DisplayItem<std::string, uint8_t, 8, 2>;

// Wide format
using WideItem = DisplayItem<std::string, int, 15, 6>;
```

## Usage Examples

### Basic Inventory System

```cpp
#include "LCDInventoryController.h"
#include "DisplayConfig.h"
#include "ConsoleRenderer.h"

// Create items with COMPILE-TIME width guarantee
// All items in this vector MUST be 11-char key, 3-char value
std::vector<InventoryDisplayItem> items;
items.emplace_back("Sword", static_cast<uint8_t>(5));
items.emplace_back("Potion", static_cast<uint8_t>(10));

// ❌ Cannot add item with different width - COMPILE ERROR!
// DisplayItem<std::string, uint8_t, 10, 3> wrongItem("Gold", 99);
// items.push_back(wrongItem);  // Won't compile!

// Create renderer
auto renderer = std::make_shared<ConsoleRenderer>();

// Create config (widths now redundant, kept for other settings)
DisplayConfig config(2, 16, 11, 3, '>', ':');

// Create controller
InventoryController controller(items, renderer, config);

// Use it
controller.render();
controller.incrementValue();
controller.navigateDown();
controller.decrementValue();
```

### Custom Width Format

```cpp
// Define a custom display format with compile-time widths
using CustomItem = DisplayItem<std::string, int, 9, 5>;

std::vector<CustomItem> sensors;
sensors.emplace_back("Temp", 72);
sensors.emplace_back("Humidity", 45);

auto renderer = std::make_shared<ConsoleRenderer>();
DisplayConfig config(2, 16, 9, 5, '*', '=');

LCDDisplayController<CustomItem> display(sensors, renderer, config);
display.render();
```

### Custom Renderer

```cpp
class FileRenderer : public IRenderer {
public:
    void render(const std::vector<std::string>& lines, size_t columns) override {
        std::ofstream file("display.txt");
        for (const auto& line : lines) {
            file << line << "\n";
        }
    }
    
    void clear() override {
        // Clear file or do nothing
    }
};

auto fileRenderer = std::make_shared<FileRenderer>();
InventoryController controller(items, fileRenderer);
```

## Backward Compatibility

The `SimulateLCDInventoryController` class maintains compatibility with older `DisplayRow`-based code:

```cpp
std::vector<DisplayRow> rows;
rows.emplace_back(11, 3);
rows[0].setKey("Health");
rows[0].setValue("100");

SimulateLCDInventoryController controller(rows);
controller.render();
```

## Extending the Library

### Creating Custom Display Formats

```cpp
// Define your own display format with specific widths
using ProductDisplay = DisplayItem<std::string, double, 8, 6>;

std::vector<ProductDisplay> products;
products.emplace_back("PROD001", 19.99);
products.emplace_back("PROD002", 29.99);

// The type system guarantees all products have 8-char IDs and 6-char prices
auto renderer = std::make_shared<ConsoleRenderer>();
DisplayConfig config(2, 16, 8, 6, '>', ':');

LCDDisplayController<ProductDisplay> controller(products, renderer, config);
```

### Creating Custom Controllers

```cpp
template<typename TDisplayItem>
class CustomController : public LCDDisplayController<TDisplayItem> {
public:
    using LCDDisplayController<TDisplayItem>::LCDDisplayController;
    
    void customOperation() {
        // Add your custom logic
        auto currentValue = this->getCurrentValue();
        this->setCurrentValue(currentValue * 2);
        this->render();
    }
};
```

## Benefits of This Design

### Compile-Time Width Enforcement ⭐ NEW
- **Items with different widths are different types** - impossible to mix
- **Compiler catches width mismatches** before runtime
- **Zero overhead** - widths optimized away at compile-time
- **Self-documenting** - type signature shows exact format
- See `COMPILE_TIME_WIDTH_DESIGN.md` for detailed explanation

### Consistency Guarantee
- All items in a vector **must** have identical widths (enforced by type system)
- Single source of truth for formatting

### Genericity
- Works with any key-value types
- Not locked into specific use cases
- Template-based for type safety and performance

### Testability
- Dependency injection enables mocking
- Interfaces allow test doubles
- No hard dependencies on I/O

### Maintainability
- Clear separation of concerns
- Each class has a single responsibility
- Easy to understand and modify

### Extensibility
- Add new renderers without touching core logic
- Create specialized controllers through composition
- Configure behavior through DisplayConfig

### Reusability
- Extract library for use in other projects
- No coupling to specific domains
- Generic enough for various applications

## File Structure

### Core Library Files

**Template Headers (Header-Only):**
```
DisplayItem.h                - Generic templated key-value item with compile-time widths
LCDDisplayController.h       - Generic display controller (template)
LCDInventoryController.h     - Inventory-specific controller (template)
```

**Configuration & Interfaces:**
```
DisplayConfig.h              - Configuration structure (simple struct)
IRenderer.h                  - Renderer interface
IInventoryController.h       - Inventory controller interface
IInputListener.h             - Input listener interface
```

**Implementation Files (.h + .cpp):**
```
ConsoleRenderer.h/cpp        - Console rendering implementation
ConsoleInputListener.h/cpp   - Console input handling
```

**Application:**
```
main.cpp                     - Example application
```

**Documentation:**
```
README.md                        - This file
COMPILE_TIME_WIDTH_DESIGN.md    - Detailed explanation of width enforcement
CODE_CLEANUP.md                  - File organization and cleanup history
BUGFIXES.md                      - Bug fix documentation
```

### Why Some Files Are Header-Only

- **Template classes** must be header-only (compiler needs full definition)
- **Interfaces** are header-only (no implementation)
- **Simple structs** can be header-only (minimal code)

## Requirements

- C++11 or later (for templates, smart pointers, static_assert)
- CMake 3.10.0 or later
- Standard library support

## Thread Safety

This library is **not** thread-safe by default. If you need concurrent access:
- Add mutex protection around controller operations
- Or ensure single-threaded access through your application architecture

## Performance

- Template instantiation happens at compile time (zero runtime overhead)
- Virtual function calls only for rendering (Strategy pattern)
- Minimal allocations (most data passed by reference)
- String formatting only when rendering

## Future Enhancements

Potential improvements while maintaining the current design:
- Add event callbacks (on navigation, value change, etc.)
- Support for scrolling displays (more items than rows)
- Animation support in renderers
- Multi-column displays
- Validation callbacks for value changes
