#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


// Represents one purchasable item.
class Item {
// Store the item's name and price as private member variables.
// Notes:
//  - Trailing underscores distinguish members from parameters/local variables.
//  - Price is stored as integer cents to avoid floating-point rounding issues.
private:
    std::string name_;
    std::int64_t price_cents_;

public:
    // Constructor (member initializer list)
    // Notes:
    //  - Take name by value because Item stores its own string, 
    //    then move it into name_ to avoid an extra copy. 
    //  - price_cents is passed by value because integers are cheap to copy. 
    //  - Validate constructor arguments so every Item starts valid.
    Item(std::string name, std::int64_t price_cents)
        : name_(std::move(name)),
          price_cents_(price_cents)
    {
        if (name_.empty()) {
            throw std::invalid_argument(
                "Item name cannot be empty."
            );
        }

        if (price_cents <= 0) {
            throw std::invalid_argument(
                "Item price must be greater than zero."
            );
        }
    }

    // Return the Item's name.
    // Notes:
    //  - Use return by const reference to avoid copying the string
    //    and prevent callers from modifying it through the getter.
    //  - trailing const means this method does not modify the Item object.
    //  - noexcept guarantees that this getter does not throw exceptions.
    const std::string& name() const noexcept
    {
        return name_;
    }

    // Return the Item's price.
    // Notes:
    //  - Return by value because std::int64_t is cheap to copy.
    //  - trailing const means this method does not modify the Item object.
    //  - noexcept guarantees that this getter does not throw exceptions.
    std::int64_t price() const noexcept
    {
        return price_cents_;
    }
};


// Order is responsible only for managing its items 
// and calculating their subtotal.
class Order {
private:
    std::vector<Item> items_;

public:
    // Take Item by value because Order stores its own Item object.
    // Move it into the vector to avoid an additional copy.
    // Notes:
    //  - Take Item by value so the function gets its own local object.
    //  - If the caller passes a normal lvalue, that parameter is copied from it.
    //  - Move the local parameter into the vector to avoid another full copy.
    //  - The original outside object remains unchanged.
    void add_item(Item item)
    {
        items_.push_back(std::move(item));
    }

    // Calculate subtotal.
    // Notes:
    //  - Initialize the running total to zero using brace initialization.
    //  - Iterate over items by const reference to avoid copying each Item
    //    and prevent modification while calculating the subtotal.
    //  - Trailing const means subtotal() does not modify the Order.
    std::int64_t subtotal() const
    {
        std::int64_t total{0};

        for (const Item& item : items_) {
            total += item.price();
        }

        return total;
    }

    // Expose items as read-only without copying the vector.
    // Notes:
    //  - Return items_ by const reference to avoid copying the vector
    //    and prevent modification through the returned reference.
    //  - The reference refers to the actual member object.
    //  - Trailing const means this getter does not modify the Order.
    //  - noexcept guarantees that it does not throw.
    const std::vector<Item>& items() const noexcept
    {
        return items_;
    }
};


// Abstract interface for discount strategies.
class DiscountPolicy {
public:
    // The virtual destructor makes destruction through a base pointer/reference
    // safe.
    virtual ~DiscountPolicy() = default;

    // Every concrete discount policy determines how much
    // should be deducted from the subtotal.
    // Notes:
    //  - virtual enables runtime polymorphism, and = 0 makes discount()
    //    pure virtual so every concrete discount policy must implement it.
    //  - subtotal_cents is passed by value because std::int64_t is cheap 
    //    to copy.
    //  - Trailing const means calculating a discount does not modify the 
    //    policy object.
    virtual std::int64_t discount(
        std::int64_t subtotal_cents
    ) const = 0;
};


// Concrete discount strategy representing "no discount".
// Notes:
//  - override verifies that this method implements DiscountPolicy::discount().
//  - The subtotal parameter is unused because this policy always returns 0.
class NoDiscount : public DiscountPolicy {
public:
    std::int64_t discount(
        std::int64_t subtotal_cents
    ) const override
    {
        return 0;
    }
};


// Concrete discount strategy that applies a percentage to the subtotal.
class PercentageDiscount : public DiscountPolicy {
private:
    int percentage_;

public:
    // Notes:
    //  - explicit prevents unintended implicit conversion from int to 
    //    PercentageDiscount.
    //  - percentage_ is initialized with a member initializer list and 
    //    validated so the object cannot be created with an invalid percentage.
    explicit PercentageDiscount(int percentage)
        : percentage_(percentage)
    {
        if (percentage < 0 || percentage > 100) {
            throw std::invalid_argument(
                "Discount percentage must be between 0 and 100."
            );
        }
    }

    // Notes:
    //  - discount() overrides the base interface and uses integer arithmetic 
    //    on cents;
    //  - Trailing const means calculating the discount does not modify the 
    //    policy.
    std::int64_t discount(
        std::int64_t subtotal_cents
    ) const override
    {
        return subtotal_cents * percentage_ / 100;
    }
};


// Concrete discount strategy that subtracts a fixed amount.
class FixedDiscount : public DiscountPolicy {
private:
    std::int64_t discount_cents_;

public:
    // Notes:
    //  - explicit prevents unintended implicit conversion from an integer.
    //  - The constructor validates the discount so it cannot be negative.
    explicit FixedDiscount(std::int64_t discount_cents)
        : discount_cents_(discount_cents)
    {
        if (discount_cents < 0) {
            throw std::invalid_argument(
                "Fixed discount cannot be negative."
            );
        }
    }

    // Notes:
    //  - discount() overrides the base interface 
    //  - Trailing const ensures that the method does not modify the policy.
    std::int64_t discount(
        std::int64_t subtotal_cents
    ) const override
    {
        // Prevent the discount from making the total negative.
        return std::min(discount_cents_, subtotal_cents);
    }
};


// Responsible only for calculating the final order total.
class OrderCalculator {
public:
    // Notes:
    //  - Accept Order and DiscountPolicy by const reference to avoid copying
    //    and prevent modification through this function.
    //  - Calls are made on the original outside objects.
    //  - DiscountPolicy& preserves runtime polymorphism, so the concrete
    //    discount implementation is selected automatically.
    std::int64_t total(
        const Order& order,
        const DiscountPolicy& discount_policy
    ) const
    {
        const std::int64_t subtotal_cents = order.subtotal();
        const std::int64_t discount_cents =
            discount_policy.discount(subtotal_cents);

        return subtotal_cents - discount_cents;
    }
};


int main()
{
    try {
        Order order;

        order.add_item(
            Item{"Keyboard", 7999}
        );

        order.add_item(
            Item{"Mouse", 3999}
        );

        order.add_item(
            Item{"USB Cable", 1299}
        );

        PercentageDiscount percentage_discount{10};
        FixedDiscount fixed_discount{2000};
        NoDiscount no_discount;

        OrderCalculator calculator;

        std::cout << std::fixed
                  << std::setprecision(2);

        std::cout << "Subtotal: "
                  << order.subtotal() / 100.0
                  << '\n';

        std::cout << "No discount: "
                  << calculator.total(
                         order,
                         no_discount
                     ) / 100.0
                  << '\n';

        std::cout << "10% discount: "
                  << calculator.total(
                         order,
                         percentage_discount
                     ) / 100.0
                  << '\n';

        std::cout << "20.00 fixed discount: "
                  << calculator.total(
                         order,
                         fixed_discount
                     ) / 100.0
                  << '\n';
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
