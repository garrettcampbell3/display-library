#ifndef IInputListener_h
#define IInputListener_h

// Navigation commands that can be triggered by input sources
enum class NavigationCommand
{
    Up,
    Down,
    Select,
    Deselect,
    Increment,
    Decrement,
    None
};

// Interface for receiving input events from various sources
// (buttons, command-line, serial, etc.)
class IInputListener
{
public:
    virtual ~IInputListener() = default;

    // Start listening for input (may be blocking or non-blocking depending on implementation)
    virtual void startListening() = 0;

    // Stop listening for input
    virtual void stopListening() = 0;

    // Poll for the next available command (non-blocking)
    // Returns NavigationCommand::None if no input is available
    virtual NavigationCommand pollCommand() = 0;

    // Wait for and return the next command (blocking)
    virtual NavigationCommand waitForCommand() = 0;

    // Check if the listener is currently active
    virtual bool isListening() const = 0;
};

#endif // IInputListener_h
