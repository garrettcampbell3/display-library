#ifndef MOCKRENDERER_H
#define MOCKRENDERER_H

#include "IRenderer.h"
#include <vector>
#include <string>

/**
 * Mock renderer for unit testing.
 * Captures rendered output for verification in tests.
 */
class MockRenderer : public IRenderer
{
public:
    std::vector<std::string> lastRenderedLines;
    size_t lastColumns = 0;
    int renderCallCount = 0;
    int clearCallCount = 0;

    void render(const std::vector<std::string>& lines, size_t columns) override
    {
        lastRenderedLines = lines;
        lastColumns = columns;
        ++renderCallCount;
    }

    void clear() override
    {
        ++clearCallCount;
    }

    /**
     * Get a specific line from the last render.
     */
    const std::string& getLine(size_t index) const
    {
        return lastRenderedLines.at(index);
    }

    /**
     * Get number of lines in last render.
     */
    size_t getLineCount() const
    {
        return lastRenderedLines.size();
    }

    /**
     * Reset all tracking data.
     */
    void reset()
    {
        lastRenderedLines.clear();
        lastColumns = 0;
        renderCallCount = 0;
        clearCallCount = 0;
    }
};

#endif // MOCKRENDERER_H
