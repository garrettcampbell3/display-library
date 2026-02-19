#ifndef ConsoleInputListener_h
#define ConsoleInputListener_h

#include "IInputListener.h"
#include <atomic>
#include <string>

// Console-based input listener that reads keyboard commands
// Supports single-key input for quick navigation
class ConsoleInputListener : public IInputListener
{
public:
    ConsoleInputListener();
    ~ConsoleInputListener() override;

    void startListening() override;
    void stopListening() override;
    NavigationCommand pollCommand() override;
    NavigationCommand waitForCommand() override;
    bool isListening() const override;

    // Display the key mappings to the user
    void printHelp() const;

private:
    std::atomic<bool> m_listening;

    // Convert a character input to a navigation command
    NavigationCommand charToCommand(char c) const;
};

#endif // ConsoleInputListener_h
