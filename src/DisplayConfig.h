#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H

#include <cstddef>

/**
 * Configuration structure for LCD display parameters.
 * Widths are now controlled by DisplayItem template parameters (compile-time).
 * This struct only contains runtime display configuration.
 */
struct DisplayConfig
{
    size_t rows;
    size_t columns;
    char navigatorChar;
    char separatorChar;

    constexpr DisplayConfig(size_t rows = 2, size_t columns = 16, 
                            char navigatorChar = '>', char separatorChar = ':')
        : rows(rows), columns(columns), 
          navigatorChar(navigatorChar), separatorChar(separatorChar)
    {
    }
};

#endif // DISPLAYCONFIG_H
