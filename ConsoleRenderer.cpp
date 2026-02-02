#include "ConsoleRenderer.h"
#include <iostream>
#include <string>

void ConsoleRenderer::render(const std::vector<std::string>& lines, size_t columns)
{
    clear();
    
    // Print top border
    std::cout << "+" << std::string(columns, '-') << "+" << std::endl;
    
    // Print each line with side borders
    for (const auto& line : lines)
    {
        std::cout << "|" << line << "|" << std::endl;
    }
    
    // Print bottom border
    std::cout << "+" << std::string(columns, '-') << "+" << std::endl;
    
    // Flush output to prevent buffering issues
    std::cout.flush();
}

void ConsoleRenderer::clear()
{
    // ANSI escape code to clear screen and move cursor to home position
    std::cout << "\033[2J\033[H";
}
