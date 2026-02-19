#ifndef IRENDERER_H
#define IRENDERER_H

#include <string>
#include <vector>

/**
 * Interface for rendering display content.
 * Follows the Strategy pattern to allow different rendering implementations.
 * Separates rendering logic from business logic (Single Responsibility Principle).
 */
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    /**
     * Render a frame with the given content.
     * 
     * @param lines Vector of strings, each representing a line to display
     * @param columns Total width of the display
     */
    virtual void render(const std::vector<std::string>& lines, size_t columns) = 0;

    /**
     * Clear the display.
     */
    virtual void clear() = 0;
};

#endif // IRENDERER_H
