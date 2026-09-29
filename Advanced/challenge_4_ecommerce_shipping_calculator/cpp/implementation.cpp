#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>


// Represents the physical data of a shipment.
// Pricing behavior is deliberately kept outside this class.
// Notes:
//  - Shipment stores only shipment data and contains no pricing logic.
//  - Weight is stored as integer grams to avoid floating-point issues.
//  - explicit prevents unintended implicit conversion from an integer to Shipment.
//  - The constructor validates the weight so every Shipment starts in a valid state.
//  - weight_grams() returns by value because std::int64_t is cheap to copy;
//  - trailing const means the getter does not modify the object,
//    and noexcept is appropriate because simply returning the member cannot throw.
class Shipment {
private:
    std::int64_t weight_grams_;

public:
    explicit Shipment(std::int64_t weight_grams)
        : weight_grams_(weight_grams)
    {
        if (weight_grams <= 0) {
            throw std::invalid_argument(
                "Shipment weight must be greater than zero."
            );
        }
    }

    std::int64_t weight_grams() const noexcept
    {
        return weight_grams_;
    }
};


// Helper function that converts grams to billable whole kilograms.
// The integer formula rounds up without using floating-point arithmetic:
// 1-1000 g -> 1 kg, 1001-2000 g -> 2 kg, etc.
// Notes:
//  - std::int64_t is passed and returned by value because it is cheap to copy.
//  - noexcept is appropriate because this arithmetic cannot throw.
std::int64_t billable_kilograms(
    std::int64_t weight_grams
) noexcept
{
    return (weight_grams + 999) / 1000;
}


// Abstract strategy interface for shipping-method pricing.
// Shipping strategies know nothing about concrete destinations.
// Notes:
//  - virtual + = 0 makes the method pure virtual, so each concrete
//    shipping strategy must provide its own calculation.
//  - kilograms is passed by value because std::int64_t is cheap to copy.
//  - Trailing const means calculating the cost does not modify the strategy object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class ShippingStrategy {
public:
    virtual ~ShippingStrategy() = default;

    virtual std::int64_t cost_for_weight(
        std::int64_t kilograms
    ) const = 0;
};


// Concrete shipping strategy implementing ShippingStrategy.
// Notes:
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means calculating the cost does not modify the strategy object.
class StandardShipping : public ShippingStrategy {
public:
    std::int64_t cost_for_weight(
        std::int64_t kilograms
    ) const override
    {
        // 5.00 base + 1.50 per kg.
        return 500 + kilograms * 150;
    }
};


// Concrete express-shipping strategy implementing ShippingStrategy.
// Notes:
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means calculating the cost does not modify the strategy object.
class ExpressShipping : public ShippingStrategy {
public:
    std::int64_t cost_for_weight(
        std::int64_t kilograms
    ) const override
    {
        // 10.00 base + 3.00 per kg.
        return 1000 + kilograms * 300;
    }
};


// Concrete international-shipping strategy implementing ShippingStrategy.
// Notes:
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means calculating the cost does not modify the strategy object.
class InternationalShipping : public ShippingStrategy {
public:
    std::int64_t cost_for_weight(
        std::int64_t kilograms
    ) const override
    {
        // 15.00 base + 3.50 per kg.
        return 1500 + kilograms * 350;
    }
};


// Separate abstract strategy interface for destination-dependent pricing.
// Destination policies know nothing about concrete shipping methods.
// Notes:
//  - virtual + = 0 makes the method pure virtual, so each concrete destination
//    policy must provide its own pricing rule.
//  - kilograms is passed by value because std::int64_t is cheap to copy.
//  - Trailing const means calculating the surcharge does not modify the policy.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class DestinationPolicy {
public:
    virtual ~DestinationPolicy() = default;

    virtual std::int64_t surcharge(
        std::int64_t kilograms
    ) const = 0;
};


// Concrete domestic destination policy implementing DestinationPolicy.
// Notes:
//  - Domestic shipping adds no extra surcharge, so the parameter is intentionally 
//    unused.
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means the policy object is not modified.
class DomesticDestination : public DestinationPolicy {
public:
    std::int64_t surcharge(
        std::int64_t
    ) const override
    {
        return 0;
    }
};


// Concrete European destination policy implementing DestinationPolicy.
// Notes:
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means calculating the surcharge does not modify the policy.
class EuropeDestination : public DestinationPolicy {
public:
    std::int64_t surcharge(
        std::int64_t kilograms
    ) const override
    {
        // 4.00 destination fee + 1.00 per kg.
        return 400 + kilograms * 100;
    }
};


// Concrete overseas destination policy implementing DestinationPolicy.
// Notes:
//  - override verifies that the base interface is implemented correctly.
//  - Trailing const means calculating the surcharge does not modify the policy.
class OverseasDestination : public DestinationPolicy {
public:
    std::int64_t surcharge(
        std::int64_t kilograms
    ) const override
    {
        // 10.00 destination fee + 2.50 per kg.
        return 1000 + kilograms * 250;
    }
};


// Context that combines two independent pricing policies:
//  1. selected shipping method
//  2. selected destination
// Neither abstraction needs to know the concrete type of the other.
class ShippingCalculator {
private:
    // Non-owning pointer to a read-only ShippingStrategy.
    // The pointer can be reseated, but the strategy cannot be modified through it.
    const ShippingStrategy* strategy_;

public:
    // Constructor.
    // Notes:
    //  - Accept the strategy by const reference to avoid copying and prevent 
    //    modification.
    //  - Store the address of that outside strategy object in the non-owning pointer
    //    member.
    //  - The referenced strategy must remain alive while ShippingCalculator uses it.
    explicit ShippingCalculator(
        const ShippingStrategy& strategy
    )
        : strategy_(&strategy)
    {
    }

    // Accept new strategy.
    // Notes:
    //  - Accept by const reference to avoid copying and prevent modification.
    //  - Store its address in the non-owning pointer so the active strategy 
    //    can be changed.
    //  - noexcept is appropriate because assigning a pointer value cannot throw.
    void set_strategy(
        const ShippingStrategy& strategy
    ) noexcept
    {
        strategy_ = &strategy;
    }

    // Calculate total costs.
    // Notes:
    //  - Accept Shipment and DestinationPolicy by const reference to avoid copying
    //    and prevent modification through this function.
    //  - References express that valid objects are required, so nullptr checks are 
    //    unnecessary.
    //  - DestinationPolicy& also preserves runtime polymorphism for concrete policies.
    std::int64_t calculate(
        const Shipment& shipment,
        const DestinationPolicy& destination
    ) const
    {
        const std::int64_t kilograms =
            billable_kilograms(
                shipment.weight_grams()
            );

        const std::int64_t shipping_cost =
            strategy_->cost_for_weight(kilograms);

        const std::int64_t destination_cost =
            destination.surcharge(kilograms);

        return shipping_cost + destination_cost;
    }
};


int main()
{
    try {
        Shipment domestic_shipment{2500};
        Shipment european_shipment{1200};
        Shipment overseas_shipment{3500};

        StandardShipping standard;
        ExpressShipping express;
        InternationalShipping international;

        DomesticDestination domestic;
        EuropeDestination europe;
        OverseasDestination overseas;

        ShippingCalculator calculator{standard};

        std::cout << std::fixed
                  << std::setprecision(2);

        std::cout << "Domestic standard: "
                  << calculator.calculate(
                         domestic_shipment,
                         domestic
                     ) / 100.0
                  << '\n';

        calculator.set_strategy(express);

        std::cout << "Europe express: "
                  << calculator.calculate(
                         european_shipment,
                         europe
                     ) / 100.0
                  << '\n';

        calculator.set_strategy(international);

        std::cout << "Overseas international: "
                  << calculator.calculate(
                         overseas_shipment,
                         overseas
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
