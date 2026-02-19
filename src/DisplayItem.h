#ifndef DISPLAYITEM_H
#define DISPLAYITEM_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

/**
 * Generic templated class with COMPILE-TIME width enforcement.
 * Widths are template parameters, making them part of the type.
 * This guarantees at compile-time that all items in a vector have identical widths.
 * 
 * @tparam TKey The type of the key (e.g., std::string, int)
 * @tparam TValue The type of the value (e.g., uint8_t, int, std::string)
 * @tparam KeyWidth The width of the key field (compile-time constant)
 * @tparam ValueWidth The width of the value field (compile-time constant)
 */
template<typename TKey, typename TValue, size_t KeyWidth, size_t ValueWidth>
class DisplayItem
{
private:
    TKey key;
    TValue value;

    // Convert any type to string for display purposes
    template<typename T>
    std::string toString(const T& data) const
    {
        std::ostringstream oss;
        oss << data;
        return oss.str();
    }

    // Specialization for uint8_t/unsigned char to display as number, not character
    std::string toString(const uint8_t& data) const
    {
        std::ostringstream oss;
        oss << static_cast<unsigned int>(data);
        return oss.str();
    }

    // Specialization for int8_t/signed char to display as number, not character
    std::string toString(const int8_t& data) const
    {
        std::ostringstream oss;
        oss << static_cast<int>(data);
        return oss.str();
    }

    // Format string to fit within specified width (pad or truncate)
    std::string formatToWidth(const std::string& text, size_t width) const
    {
        if (text.length() < width)
        {
            return text + std::string(width - text.length(), ' ');
        }
        else if (text.length() > width)
        {
            return text.substr(0, width);
        }
        return text;
    }

public:
    DisplayItem()
        : key(TKey{}), value(TValue{})
    {
    }

    DisplayItem(const TKey& key, const TValue& value)
        : key(key), value(value)
    {
    }

    void setKey(const TKey& newKey)
    {
        key = newKey;
    }

    void setValue(const TValue& newValue)
    {
        value = newValue;
    }

    TKey getKey() const
    {
        return key;
    }

    TValue getValue() const
    {
        return value;
    }

    // Get formatted key string for display
    std::string getFormattedKey() const
    {
        return formatToWidth(toString(key), KeyWidth);
    }

    // Get formatted value string for display
    std::string getFormattedValue() const
    {
        return formatToWidth(toString(value), ValueWidth);
    }

    // Compile-time accessors for widths
    static constexpr size_t getKeyWidth() { return KeyWidth; }
    static constexpr size_t getValueWidth() { return ValueWidth; }
    static constexpr size_t getTotalWidth() { return KeyWidth + ValueWidth; }
};

// Type alias for common inventory use case: string key, uint8_t value, 11x3 display
using InventoryDisplayItem = DisplayItem<std::string, uint8_t, 11, 3>;

#endif // DISPLAYITEM_H
