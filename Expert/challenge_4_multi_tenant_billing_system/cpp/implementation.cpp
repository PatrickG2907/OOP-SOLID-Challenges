// =============================================================================
// Architecture Overview
// =============================================================================
// This billing system demonstrates the Strategy Pattern, Factory Pattern,
// and runtime polymorphism.
//
// Core flow:
//  1. Customer stores identity, usage, and the selected PricingModelType.
//  2. BillingService asks PricingStrategyFactory to create the matching
//     PricingStrategy.
//  3. PricingStrategyFactory constructs and configures the appropriate
//     concrete strategy.
//  4. BillingService calculates the amount through the PricingStrategy
//     interface without depending on the concrete pricing implementation.
//  5. The calculated amount and customer ID are stored in an Invoice.
//
// Main components:
//  - Customer:
//      Holds customer data, usage, and the selected pricing model.
//
//  - Invoice:
//      Represents the result of billing a customer.
//
//  - PricingStrategy:
//      Abstract interface for pricing calculations.
//
//  - FlatRatePricing:
//      Charges a fixed amount regardless of usage.
//
//  - UsageBasedPricing:
//      Charges a configured amount per usage unit.
//
//  - Tier:
//      Small value object representing one tier threshold and unit price.
//
//  - TieredPricing:
//      Applies progressive pricing across a configured collection of tiers.
//
//  - PricingStrategyFactory:
//      Centralizes creation and configuration of concrete pricing strategies.
//      Pricing values are predefined here to keep the exercise focused on
//      Factory and Strategy behavior.
//
//  - BillingService:
//      Coordinates strategy creation, price calculation, and Invoice creation.
//
// Ownership:
//  - Customer and Invoice own their string data.
//  - TieredPricing owns its std::vector<Tier>.
//  - PricingStrategyFactory returns strategies through
//    std::unique_ptr<PricingStrategy>.
//  - BillingService owns each created strategy only for the duration of one
//    invoice-generation call.
//
// Design intent:
//  - Pricing behavior is separated from billing orchestration.
//  - Concrete pricing algorithms can vary behind one common interface.
//  - BillingService remains independent of concrete strategy classes.


// =============================================================================
// Includes
// =============================================================================

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


// =============================================================================
// Domain Model
// =============================================================================

// Strongly typed identifiers for the supported pricing models.
// Notes:
//  - Using enum class keeps pricing-model selection type-safe and avoids
//    typo-prone string identifiers.
//  - The values represent the concrete pricing strategies the factory can 
//    create.
enum class PricingModelType {
    FlatRate,
    UsageBased,
    Tiered
};


// Represents a customer that can be billed using one of the supported pricing 
// models.
// Notes:
//  - id_ and name_ are owned std::string values stored by the Customer.
//  - usage_units_ stores the customer's measured usage for the billing period.
//  - pricing_model_ identifies which pricing strategy should be used for this 
//    customer.
//  - validate_id() and validate_name() enforce that every Customer has
//    a non-empty ID and name.
//  - usage_units_ may be zero, so no additional validation is required for it.
class Customer {
private:
    std::string id_;
    std::string name_;
    std::size_t usage_units_;
    PricingModelType pricing_model_;
    
    // Validate that the customer ID is not empty.
    // Note:
    //  - static is appropriate because the check depends only on the supplied
    //    value and does not require access to Customer state.
    static void validate_id(std::string_view id) {
        if (id.empty()) {
            throw std::invalid_argument("ID must not be empty!");
        }
    }
    
    // Validate that the customer name is not empty.
    // Note:
    //  - static is appropriate because the check depends only on the supplied 
    //    value and does not require access to Customer state.
    static void validate_name(std::string_view name) {
        if (name.empty()) {
            throw std::invalid_argument("Name must not be empty!");
        }
    }
    
public:
    // Construct a Customer with identity, usage information, and pricing model.
    // Notes:
    //  - id and name are taken by value because Customer stores its own copies.
    //  - std::move transfers the local string resources into the owned members.
    //  - usage_units and pricing_model are small value types and are copied.
    //  - Validation is performed on the stored string members after ownership
    //    has been transferred into the object.
    Customer(
        std::string id,
        std::string name,
        std::size_t usage_units,
        PricingModelType pricing_model
    ) 
        : id_(std::move(id)),
          name_(std::move(name)),
          usage_units_(usage_units),
          pricing_model_(pricing_model)
    {
        validate_id(id_);
        validate_name(name_);
    }
    
    // Provide read-only access to the customer ID without copying it.
    const std::string& id() const noexcept {
        return id_;
    }
    
    // Provide read-only access to the customer name without copying it.
    const std::string& name() const noexcept {
        return name_;
    }
    
    // Return the customer's usage amount by value.
    std::size_t usage_units() const noexcept {
        return usage_units_;
    }
    
    // Return the customer's selected pricing model by value.
    PricingModelType pricing_model() const noexcept {
        return pricing_model_;
    }
};


// Represents the result of billing a customer for a billing period.
// Notes:
//  - customer_id_ identifies which Customer the invoice belongs to.
//  - amount_cents_ stores the invoice amount as integer cents instead of
//    floating-point money.
//  - validate_customer_id() enforces that every Invoice belongs to a
//    non-empty customer ID.
//  - validate_amount_cents() enforces that invoice amounts are never negative.
class Invoice {
private:
    std::string customer_id_;
    std::int64_t amount_cents_;

    // Validate that the customer ID is not empty.
    // Note:
    //  - static is appropriate because the check depends only on the supplied
    //    value and does not require access to Invoice state.
    static void validate_customer_id(std::string_view customer_id) {
        if (customer_id.empty()) {
            throw std::invalid_argument("Customer ID must not be empty!");
        }
    }
    
    // Validate that the calculated invoice amount is not negative.
    // Note:
    //  - A zero amount is allowed because a customer may legitimately 
    //    owe nothing.
    static void validate_amount_cents(std::int64_t amount_cents) {
        if (amount_cents < 0) {
            throw std::invalid_argument("Invoice amount must not be negative!");
        }
    }
    
public:
    // Construct an Invoice for a customer with a calculated amount.
    // Notes:
    //  - customer_id is taken by value because Invoice stores its own copy.
    //  - std::move transfers the local string resources into customer_id_.
    //  - amount_cents is a small integer value and is copied.
    //  - Validation is performed on the stored members after initialization.
    Invoice(std::string customer_id, std::int64_t amount_cents)
        : customer_id_(std::move(customer_id)), 
          amount_cents_(amount_cents)
    {
        validate_customer_id(customer_id_);
        validate_amount_cents(amount_cents_);
    }
    
    // Provide read-only access to the customer ID without copying it.
    const std::string& customer_id() const noexcept {
        return customer_id_;
    }
    
    // Return the invoice amount in cents by value.
    std::int64_t amount_cents() const noexcept {
        return amount_cents_;
    }
};


// =============================================================================
// Pricing Strategy Interface
// =============================================================================

// Abstract strategy interface for calculating invoice amounts.
// Notes:
//  - Concrete pricing models implement calculate() with their own pricing 
//    rules.
//  - usage_units is passed by value because std::size_t is a small value type.
//  - The return value represents the calculated price in integer cents.
//  - calculate() is const because pricing calculations should not modify
//    the strategy object's state.
//  - The virtual destructor ensures safe destruction through PricingStrategy 
//    pointers.
class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    
    virtual std::int64_t calculate(std::size_t usage_units) const = 0;
};


// =============================================================================
// Pricing Strategy Implementations
// =============================================================================

// Concrete pricing strategy that charges a fixed amount regardless of usage.
// Notes:
//  - Inherits from PricingStrategy and provides the required calculate() 
//    implementation.
//  - flat_rate_cents_ stores the configured flat price in integer cents.
//  - validate_flat_rate_cents() enforces that the configured price is not 
//    negative.
//  - The constructor stores the configured rate by value because std::int64_t
//    is a small scalar type.
//  - calculate() ignores usage_units because flat-rate pricing always returns
//    the same configured amount.
//  - calculate() is const because determining the price does not modify
//    the strategy object's state.
class FlatRatePricing : public PricingStrategy {
private:
    std::int64_t flat_rate_cents_;
    
    // Validate that the configured flat rate is not negative.
    // Note:
    //  - A zero rate is allowed because a free flat-rate plan may be valid.
    static void validate_flat_rate_cents(std::int64_t flat_rate_cents) {
        if (flat_rate_cents < 0) {
            throw std::invalid_argument("Flat rate price must not be negative!");
        }
    }

public:
    // Construct the strategy with its configured flat-rate amount.
    explicit FlatRatePricing(std::int64_t flat_rate_cents) 
        : flat_rate_cents_(flat_rate_cents)
    {
        validate_flat_rate_cents(flat_rate_cents_);
    }
    
    // Return the configured flat rate regardless of the supplied usage amount.
    // Note:
    //  - The parameter name is intentionally omitted because this strategy
    //    does not use usage_units.
    std::int64_t calculate(std::size_t) const override {
        return flat_rate_cents_;
    }
};


// Concrete pricing strategy that charges for each unit of usage.
// Notes:
//  - Inherits from PricingStrategy and provides the required calculate()
//    implementation.
//  - price_per_unit_cents_ stores the configured price for one usage unit.
//  - validate_price_per_unit_cents() enforces that the configured unit price
//    is not negative.
//  - A zero unit price is allowed because a free usage-based plan may be valid.
//  - calculate() multiplies the supplied usage amount by the configured
//    price per unit.
//  - The calculation is checked before multiplication so an invoice amount
//    cannot silently overflow std::int64_t.
//  - calculate() is const because determining the price does not modify
//    the strategy object's state.
class UsageBasedPricing : public PricingStrategy {
private:
    std::int64_t price_per_unit_cents_;
    
    // Validate that the configured price per usage unit is not negative.
    // Note:
    //  - static is appropriate because the check depends only on the supplied
    //    value and does not require access to object state.
    static void validate_price_per_unit_cents(
        std::int64_t price_per_unit_cents) {
        if (price_per_unit_cents < 0) {
            throw std::invalid_argument("Price per unit must not be negative!");
        }
    }
    
public:
    // Construct the strategy with its configured per-unit price.
    // Notes:
    //  - price_per_unit_cents is passed by value because std::int64_t
    //    is a small scalar type.
    //  - explicit prevents unintended implicit construction from an integer.
    explicit UsageBasedPricing(std::int64_t price_per_unit_cents)
        : price_per_unit_cents_(price_per_unit_cents)
    {
        validate_price_per_unit_cents(price_per_unit_cents_);
    }
        
    // Calculate the invoice amount from usage.
    // Notes:
    //  - Zero usage or a zero unit price produces a zero invoice amount.
    //  - usage_units and price_per_unit_cents_ are converted to std::uintmax_t
    //    because both values are known to be non-negative.
    //  - max_amount represents the largest value that can safely be returned
    //    as std::int64_t.
    //  - The division-based check detects overflow before multiplication occurs.
    //  - std::overflow_error is thrown if the calculated amount would exceed
    //    the supported monetary range.
    std::int64_t calculate(std::size_t usage_units) const override {
        if (usage_units == 0 || price_per_unit_cents_ == 0) {
            return 0;
        }
        const std::uintmax_t usage =
            static_cast<std::uintmax_t>(usage_units);
    
        const std::uintmax_t price =
            static_cast<std::uintmax_t>(price_per_unit_cents_);
    
        const std::uintmax_t max_amount =
            static_cast<std::uintmax_t>(
                std::numeric_limits<std::int64_t>::max()
            );
    
        if (usage > max_amount / price) {
            throw std::overflow_error(
                "Calculated invoice amount exceeds supported range!"
            );
        }
    
        return static_cast<std::int64_t>(usage * price);
    }
};


// Small value object that represents one tier in a tiered pricing model.
// Notes:
//  - start_units_ defines the usage amount at which this tier becomes active.
//  - price_per_unit_cents_ stores the per-unit price for this tier in integer
//    cents.
//  - validate_price_per_unit_cents() enforces that a tier price is never 
//    negative.
//  - Tier validates only its own local state; relationships between multiple 
//    tiers, such as ordering and duplicate start values, are handled by 
//    TieredPricing.
class Tier {
private:
    std::size_t start_units_;
    std::int64_t price_per_unit_cents_;
    
    // Validate that the configured per-unit price is not negative.
    // Note:
    //  - static is appropriate because the check depends only on the supplied
    //    value and does not require access to Tier state.
    static void validate_price_per_unit_cents(
        std::int64_t price_per_unit_cents) {
        if (price_per_unit_cents < 0) {
            throw std::invalid_argument("Price per unit must not be negative!");
        }
    }
    
public:
    // Construct one tier with its starting usage threshold and unit price.
    // Notes:
    //  - start_units and price_per_unit_cents are small scalar values
    //    and are therefore stored by value.
    //  - start_units may be zero because the first tier should begin at zero.
    //  - The stored price is validated after initialization.
    Tier(std::size_t start_units, std::int64_t price_per_unit_cents)
        : start_units_(start_units), 
          price_per_unit_cents_(price_per_unit_cents)
    {
        validate_price_per_unit_cents(price_per_unit_cents_);
    }
        
    // Return the usage amount at which this tier starts.
    std::size_t start_units() const noexcept {
        return start_units_;
    } 
    
    // Return the configured per-unit price in cents.
    std::int64_t price_per_unit_cents() const noexcept {
        return price_per_unit_cents_;
    }
};


// Concrete pricing strategy that applies progressive usage tiers.
// Notes:
//  - tiers_ stores the configured tier structure owned by this strategy.
//  - validate_tiers() ensures the configuration is usable before any
//    calculation is performed.
//  - The first tier must start at zero so every usage value is covered.
//  - Tier start values must be strictly increasing to avoid overlaps
//    and ambiguous ranges.
//  - calculate() applies each tier's price only to the usage units that
//    fall within that tier.
//  - The next tier's start value acts as the current tier's upper boundary.
//  - The final tier has no fixed upper boundary and extends to usage_units.
//  - This implementation intentionally keeps the arithmetic simple because
//    the main focus of the exercise is Strategy, Factory, and Polymorphism.
class TieredPricing : public PricingStrategy {
private:
    std::vector<Tier> tiers_;
    
    // Validate relationships between the configured tiers.
    // Note:
    //  - Individual Tier objects already validate their own per-unit prices.
    static void validate_tiers(const std::vector<Tier>& tiers) {
        if (tiers.empty()) {
            throw std::invalid_argument("Tiers must not be empty!");
        }
        if (tiers.front().start_units() != 0) {
            throw std::invalid_argument("First start unit must be zero!");
        }
        for (std::size_t i = 1; i < tiers.size(); ++i) {
            if (tiers[i - 1].start_units() >= tiers[i].start_units()) {
                throw std::invalid_argument("Start units must be strictly increasing!");
            }
        }
    }
    
public:
    // Takes the tier configuration by value because TieredPricing owns it,
    // then moves it into the member to avoid an additional copy.
    explicit TieredPricing(std::vector<Tier> tiers)
        : tiers_(std::move(tiers))
    {
        validate_tiers(tiers_);
    }
        
    // Calculate the total price by progressively charging each usage range
    // with the rate configured for its tier.
    std::int64_t calculate(std::size_t usage_units) const override {
        std::int64_t total = 0;
    
        for (std::size_t i = 0; i < tiers_.size(); ++i) {
            const std::size_t start = tiers_[i].start_units();
    
            // No later tier can contribute once usage does not reach
            // the current tier's starting threshold.
            if (usage_units <= start) {
                break;
            }
    
            // The next tier's start is the current tier's upper boundary.
            // Note:
            //  - The last tier simply extends to the customer's total usage.
            const std::size_t end =
                (i + 1 < tiers_.size())
                    ? std::min(usage_units, tiers_[i + 1].start_units())
                    : usage_units;
    
            const std::size_t units = end - start;
    
            total += static_cast<std::int64_t>(units)
                   * tiers_[i].price_per_unit_cents();
        }
    
        return total;
    }
};


// =============================================================================
// Pricing Strategy Factory
// =============================================================================

// Factory responsible for creating configured pricing strategies.
// Notes:
//  - create() receives a PricingModelType and returns the corresponding
//    concrete strategy through the PricingStrategy interface.
//  - std::unique_ptr expresses exclusive ownership of the created strategy.
//  - std::make_unique constructs the concrete strategy and returns ownership
//    safely without exposing raw new/delete operations.
//  - Pricing configuration is intentionally centralized here for this exercise.
//  - The predefined values keep the focus on the Factory Pattern rather than
//    introducing a separate configuration subsystem.
//  - If an unsupported enum value is supplied, create() throws
//    std::invalid_argument.
class PricingStrategyFactory {
public:
    // Create a fully configured pricing strategy for the requested model.
    static std::unique_ptr<PricingStrategy> create(PricingModelType type) {
        switch (type) {
            case PricingModelType::FlatRate:
                return std::make_unique<FlatRatePricing>(5000);
            case PricingModelType::UsageBased:
                return std::make_unique<UsageBasedPricing>(10);
            case PricingModelType::Tiered:
                return std::make_unique<TieredPricing>(
                    std::vector<Tier>{
                        Tier(0, 10),
                        Tier(100, 7),
                        Tier(500, 5)
                    }
                );
        }
        throw std::invalid_argument("Unsupported pricing model!");
    }
};


// =============================================================================
// Billing Service
// =============================================================================

// Application service that coordinates invoice generation.
// Notes:
//  - BillingService does not implement pricing rules itself.
//  - It asks PricingStrategyFactory for the strategy selected by the Customer.
//  - The returned strategy is used polymorphically through PricingStrategy.
//  - generate_invoice() calculates the amount from the customer's usage and
//    creates the resulting Invoice.
//  - The local std::unique_ptr owns the strategy only for the duration of the call.
//  - generate_invoice() is const because the service does not modify its own state.
class BillingService {
public:
    // Generate an Invoice for the supplied Customer.
    Invoice generate_invoice(const Customer& customer) const {
        
        // Create the concrete pricing strategy selected by the customer.
        auto strategy =
            PricingStrategyFactory::create(customer.pricing_model());
            
        // Calculate the invoice amount through the common strategy interface.
        const std::int64_t amount_cents =
            strategy->calculate(customer.usage_units());
            
        // Invoice owns its own copy of the customer ID.
        return Invoice(customer.id(), amount_cents);
    }
};


// =============================================================================
// Demo / Entry Point
// =============================================================================

// Demonstrates the complete billing workflow for all supported pricing models.
// Notes:
//  - One BillingService instance is reused for every customer.
//  - std::array is used because the demo contains a fixed number of customers.
//  - Each Customer selects a different PricingModelType.
//  - The loop processes every customer through the same BillingService API.
//  - Customers are iterated by const reference to avoid unnecessary copies.
//  - Each generated Invoice is treated as read-only after creation.
int main() {
    const BillingService billing_service;

    // Create one example customer for each supported pricing model.
    const std::array<Customer, 3> customers{
        Customer("1", "Customer Flatrate", 100, PricingModelType::FlatRate),
        Customer("2", "Customer UsageBased", 200, PricingModelType::UsageBased),
        Customer("3", "Customer Tiered", 300, PricingModelType::Tiered)
    };
    
    // Generate and display an invoice for each customer using the same
    // billing workflow, regardless of the selected pricing strategy.
    for (const Customer& customer : customers) {
        const Invoice invoice = billing_service.generate_invoice(customer);
        std::cout << "Customer ID: " 
                  << invoice.customer_id() 
                  << ", Amount [cents]: "
                  << invoice.amount_cents()
                  << '\n';
    }
    return 0;
}
