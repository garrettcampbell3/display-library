#include "ConsoleInputListener.h"
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

ConsoleInputListener::ConsoleInputListener()
    : m_listening(false)
{
}

ConsoleInputListener::~ConsoleInputListener()
{
    stopListening();
}

void ConsoleInputListener::startListening()
{
    m_listening = true;
}

void ConsoleInputListener::stopListening()
{
    m_listening = false;
}

bool ConsoleInputListener::isListening() const
{
    return m_listening;
}

void ConsoleInputListener::printHelp() const
{
    std::cout << "\n=== Navigation Controls ===" << std::endl;
    std::cout << "  w / W  : Navigate Up" << std::endl;
    std::cout << "  s / S  : Navigate Down" << std::endl;
    std::cout << "  e / E  : Select Item" << std::endl;
    std::cout << "  q / Q  : Deselect Item" << std::endl;
    std::cout << "  d / D  : Increment Value" << std::endl;
    std::cout << "  a / A  : Decrement Value" << std::endl;
    std::cout << "  x / X  : Exit" << std::endl;
    std::cout << "===========================\n" << std::endl;
}

NavigationCommand ConsoleInputListener::charToCommand(char c) const
{
    switch (c)
    {
    case 'w':
    case 'W':
        return NavigationCommand::Up;
    case 's':
    case 'S':
        return NavigationCommand::Down;
    case 'e':
    case 'E':
        return NavigationCommand::Select;
    case 'q':
    case 'Q':
        return NavigationCommand::Deselect;
    case 'd':
    case 'D':
        return NavigationCommand::Increment;
    case 'a':
    case 'A':
        return NavigationCommand::Decrement;
    default:
        return NavigationCommand::None;
    }
}

NavigationCommand ConsoleInputListener::pollCommand()
{
    if (!m_listening)
    {
        return NavigationCommand::None;
    }

    if (_kbhit())
    {
        char c = static_cast<char>(_getch());
        return charToCommand(c);
    }
    return NavigationCommand::None;
}

NavigationCommand ConsoleInputListener::waitForCommand()
{
    if (!m_listening)
    {
        return NavigationCommand::None;
    }

    char c = static_cast<char>(_getch());
    return charToCommand(c);
}
