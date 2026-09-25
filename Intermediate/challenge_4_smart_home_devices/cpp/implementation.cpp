#include <iostream>
#include <stdexcept>

// Interface for devices that can be switched on and off.
// Notes:
//  - Small interface representing the switchable capability.
//  - Pure virtual methods force concrete devices to implement on/off behavior,
//    while the virtual destructor makes polymorphic destruction safe.
class Switchable {
public:
    // Virtual destructor because Switchable is used polymorphically.
    virtual ~Switchable() = default;

    // Pure virtual methods: every concrete switchable device
    // must provide its own implementation.
    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
};


// Separate interface for devices that support an adjustable value.
// This avoids forcing all switchable devices to implement set_value().
// Notes:
//  - Keeps set_value() out of Switchable so non-adjustable devices are not
//    forced to implement unused behavior. 
//  - int is passed by value because it is cheap to copy.
class Adjustable {
public:
    // Virtual destructor because Adjustable is used polymorphically.
    virtual ~Adjustable() = default;

    // Pure virtual method: adjustable devices must define
    // how their value is changed.
    virtual void set_value(int value) = 0;
};


// Light only supports switching on and off.
// It therefore implements only the Switchable interface.
// Notes:
//  - Light publicly implements the Switchable capability.
//  - is_on_ is initialized to false in-class.
//  - override verifies the inherited interface methods.
//  - The methods modify the Light's internal state.
class Light : public Switchable {
private:
    // In-class initialization: lights start turned off.
    bool is_on_{false};

public:
    void turn_on() override
    {
        is_on_ = true;
        std::cout << "Light turned on.\n";
    }

    void turn_off() override
    {
        is_on_ = false;
        std::cout << "Light turned off.\n";
    }
};


// Fan supports both switching and adjustable speed.
// Multiple inheritance is appropriate here because both base classes
// represent independent capabilities/interfaces.
// Notes:
//  - Fan implements two independent interfaces through multiple inheritance.
//  - Public inheritance allows it to be used as either Switchable or Adjustable.
//  - Members use in-class initialization.
//  - override verifies interface methods.
//  - set_value() validates input before changing the fan's internal state.
class Fan : public Switchable, public Adjustable {
private:
    // Fan starts turned off with speed 0.
    bool is_on_{false};
    int speed_{0};

public:
    void turn_on() override
    {
        is_on_ = true;
        std::cout << "Fan turned on.\n";
    }

    void turn_off() override
    {
        is_on_ = false;
        std::cout << "Fan turned off.\n";
    }

    void set_value(int speed) override
    {
        // Pass int by value because copying small numeric types is cheap.
        // Validate before modifying the object's state.
        if (speed < 0 || speed > 3) {
            throw std::invalid_argument(
                "Fan speed must be between 0 and 3."
            );
        }

        speed_ = speed;

        std::cout << "Fan speed set to "
                  << speed_
                  << ".\n";
    }
};


// Accept any device that implements Switchable.
// Notes:
//  - Pass by reference to avoid copying and preserve polymorphism.
//  - Reference is preferred over a pointer because a valid device is required
//    and nullptr handling is therefore unnecessary.
void activate(Switchable& device)
{
    device.turn_on();
}


// Accept only devices that implement Adjustable.
// A Light cannot be passed here because it does not implement Adjustable.
// Notes:
//  - Pass Adjustable by reference to avoid copying and preserve polymorphism.
//  - A reference is preferred because a valid adjustable device is required.
//  - Pass int by value because copying numeric types is cheap.
void adjust(Adjustable& device, int value)
{
    device.set_value(value);
}


int main()
{
    try {
        Light light;
        Fan fan;

        activate(light);
        activate(fan);

        adjust(fan, 2);

        light.turn_off();
        fan.turn_off();
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
