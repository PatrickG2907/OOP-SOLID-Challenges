#include <cstdint>
#include <iostream>
#include <stdexcept>

// Abstract base class for all employee types.
class Employee {
public:
    // Virtual destructor because Employee is used polymorphically.
    virtual ~Employee() = default;

    // Every concrete employee type must provide its own salary calculation.
    // Notes:
    //  - "virtual" enables polymorphic overriding.
    //  - "= 0" makes the method pure virtual, so concrete derived classes 
    //    must implement it.
    //  - trailing const means salary() does not modify the employee object.
    virtual std::int64_t salary() const = 0;
};


// Full-time employees receive a fixed monthly salary.
// Notes:
// - Constructor initializes the salary directly with a member initializer 
//   list and validates that the value is greater than zero.
// - "explicit" prevents unintended implicit conversion from an integer 
//   salary to a FullTimeEmployee object.
// - Pass the integer salary by value because copying numeric types is cheap.
// - The trailing underscore marks monthly_salary_cents_ as a member variable.
class FullTimeEmployee : public Employee {
private:
    std::int64_t monthly_salary_cents_;

public:
    explicit FullTimeEmployee(std::int64_t monthly_salary_cents)
        : monthly_salary_cents_(monthly_salary_cents)
    {
        if (monthly_salary_cents <= 0) {
            throw std::invalid_argument(
                "Monthly salary must be greater than zero."
            );
        }
    }

    std::int64_t salary() const override
    {
        return monthly_salary_cents_;
    }
};


// Part-time employees are paid according to hours worked.
class PartTimeEmployee : public Employee {
private:
    std::int64_t hourly_rate_cents_;
    int hours_worked_;

public:
    // Notes:
    //  - Constructor passes numeric values by value, initializes members 
    //    directly with a member initializer list, and validates inputs to 
    //    preserve valid state.
    //  - No explicit needed because this constructor requires two arguments.
    PartTimeEmployee(
        std::int64_t hourly_rate_cents,
        int hours_worked
    )
        : hourly_rate_cents_(hourly_rate_cents),
          hours_worked_(hours_worked)
    {
        if (hourly_rate_cents <= 0) {
            throw std::invalid_argument(
                "Hourly rate must be greater than zero."
            );
        }

        if (hours_worked < 0) {
            throw std::invalid_argument(
                "Hours worked cannot be negative."
            );
        }
    }

    std::int64_t salary() const override
    {
        return hourly_rate_cents_ * hours_worked_;
    }
};


// Contractors receive a fixed contract payment.
class Contractor : public Employee {
private:
    std::int64_t contract_payment_cents_;

public:
    // Notes:
    //  - Single-argument constructor: explicit prevents implicit conversion.
    //  - Pass numeric value by value, initialize the member directly, and 
    //    validate it.
    //  - salary() returns the integer by value because numeric types are cheap to copy.
    //  - trailing const prevents modification of the object.
    //  - override verifies that the base-class method is implemented correctly.
    explicit Contractor(std::int64_t contract_payment_cents)
        : contract_payment_cents_(contract_payment_cents)
    {
        if (contract_payment_cents <= 0) {
            throw std::invalid_argument(
                "Contract payment must be greater than zero."
            );
        }
    }

    std::int64_t salary() const override
    {
        return contract_payment_cents_;
    }
};


// Any Employee subtype can be passed here.
// Runtime polymorphism selects the correct salary() implementation.
// Notes:
//  - Pass Employee by const reference to avoid copying while preserving 
//    polymorphism.
//  - A reference is preferred over a pointer here because a valid Employee 
//    is required
//  - Pointers would allow nullptr and therefore need additional null handling.
void print_salary(const Employee& employee)
{
    std::cout << "Salary: "
              << employee.salary() / 100.0
              << '\n';
}


int main()
{
    try {
        FullTimeEmployee full_time{350000};   // 3500.00
        PartTimeEmployee part_time{2000, 80}; // 20.00 * 80
        Contractor contractor{500000};        // 5000.00

        print_salary(full_time);
        print_salary(part_time);
        print_salary(contractor);
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
