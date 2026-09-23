#include <iostream>
#include <stdexcept>

class Shape {
public:
    // Virtual destructor because Shape is used as a polymorphic base class.
    // Explanations: 
    //  - Virtual destructor ensures derived objects are destroyed correctly 
    //    when accessed and deleted through a Shape base-class pointer.
    //  - "= default" uses the compiler-generated destructor implementation.
    virtual ~Shape() = default;

    // Pure virtual function: enables polymorphism (virtual) and requires
    // concrete derived classes (=0) to provide their own implementation.
    virtual double area() const = 0;
};

// Rectangle publicly derives from Shape ("Rectangle is a Shape").
// Note: keyword "public"
//  - public members of Shape stay public in Rectangle
//  - protected members stay protected
//  - private members of Shape remain inaccessible directly
class Rectangle : public Shape {
private:
    // Trailing underscore marks these as member variables
    // and distinguishes them from parameters/local variables.
    double width_;
    double height_;

public:
    // Constructor: initializes class members with a member initializer list, 
    // and validates its dimensions.
    Rectangle(double width, double height)
        : width_(width),
          height_(height)
    {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument(
                "Width and height must be greater than zero."
            );
        }
    }
    
    // Override base class virtual method.
    // The keyword "override" tells the compiler to verify that this method
    // correctly overrides a virtual method from the base class.
    double area() const override
    {
        return width_ * height_;
    }
};

// Circle publicly derives from Shape ("Circle is a Shape").
// Note: keyword "public"
//  - public members of Shape stay public in Circle
//  - protected members stay protected
//  - private members of Shape remain inaccessible directly
class Circle : public Shape {
private:
    // Trailing underscore marks member variable.
    double radius_;

public:
    // Constructor: initializes the radius with a member initializer list,
    // and validates its value.
    // Explanation: "explicit" prevents implicit conversion from a single double 
    //              to Circle. Rectangle takes two arguments, so this implicit 
    //              conversion issue does not apply.
    explicit Circle(double radius)
        : radius_(radius)
    {
        if (radius <= 0) {
            throw std::invalid_argument(
                "Radius must be greater than zero."
            );
        }
    }

    // Override base class virtual method.
    // The keyword "override" tells the compiler to verify that this method
    // correctly overrides a virtual method from the base class.
    double area() const override
    {
        constexpr double pi = 3.1415;
        return pi * radius_ * radius_;
    }
};

// Accept any object derived from Shape.
// Runtime polymorphism determines which area() implementation is called.
// Explanation: pass by const reference (&) to avoid copying and preserve 
//              polymorphic behavior.
void print_area(const Shape& shape)
{
    std::cout << "Area: " << shape.area() << '\n';
}

int main()
{
    try {
        Rectangle rectangle(4.0, 5.0);
        Circle circle(3.0);

        print_area(rectangle);
        print_area(circle);
    }
    catch (const std::invalid_argument& error) {
        std::cerr << "Error: " << error.what() << '\n';
    }

    return 0;
}
