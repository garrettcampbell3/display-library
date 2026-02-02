#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "IRenderer.h"
#include <string>
#include <vector>

/**
 * Console-based renderer implementation.
 * Renders content to standard output with borders and formatting.
 */
class ConsoleRenderer : public IRenderer
{
public:
    void render(const std::vector<std::string>& lines, size_t columns) override;
    void clear() override;
};

#endif // CONSOLERENDERER_H
