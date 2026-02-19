#ifndef IInventoryController_h
#define IInventoryController_h

class IInventoryController
{
public:
    virtual void navigateUp() = 0;
    virtual void navigateDown() = 0;
    virtual void selectItem() = 0;
    virtual void deselectItem() = 0;
    virtual void incrementValue() = 0;
    virtual void decrementValue() = 0;
};

#endif // IInventoryController_h
