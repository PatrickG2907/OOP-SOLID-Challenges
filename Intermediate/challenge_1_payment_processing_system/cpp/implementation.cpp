#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

// Abstraction for all payment strategies.
class PaymentStrategy {
public:
    // Virtual destructor ensures derived objects are destroyed correctly
    // when owned through a PaymentStrategy base-class pointer.
    virtual ~PaymentStrategy() = default;

    // Pure virtual function: every concrete payment strategy
    // must provide its own payment implementation.
    // The trailing const on this member function guarantees that it does not modify
    // the object's non-mutable member state.
    virtual void pay(std::int64_t amount_cents) const = 0;
};


// Concrete strategy implementations.
// Public inheritance allows use through PaymentStrategy.
// amount_cents is passed by value because integer copies are cheap.
// trailing const prevents modification of the strategy object.
// override verifies the base-class method is implemented correctly.
class CreditCardPayment : public PaymentStrategy {
public:
    void pay(std::int64_t amount_cents) const override
    {
        std::cout << "Paid "
                  << amount_cents / 100.0
                  << " via Credit Card.\n";
    }
};


class PayPalPayment : public PaymentStrategy {
public:
    void pay(std::int64_t amount_cents) const override
    {
        std::cout << "Paid "
                  << amount_cents / 100.0
                  << " via PayPal.\n";
    }
};


// Concrete strategy: Crypto
class CryptoPayment : public PaymentStrategy {
public:
    void pay(std::int64_t amount_cents) const override
    {
        std::cout << "Paid "
                  << amount_cents / 100.0
                  << " via Crypto.\n";
    }
};


class PaymentProcessor {
private:
    // The processor owns exactly one payment strategy.
    // unique_ptr gives PaymentProcessor exclusive ownership of the current strategy
    // and supports polymorphism.
    // Replacing it automatically destroys the previously owned strategy,
    // avoiding manual memory management with new/delete.
    std::unique_ptr<PaymentStrategy> strategy_;

public:
    // Inject the strategy instead of depending on a concrete payment type.
    // Take unique_ptr by value because the constructor takes ownership
    // of the strategy, then move that ownership into the member variable.
    explicit PaymentProcessor(
        std::unique_ptr<PaymentStrategy> strategy
    )
        : strategy_(std::move(strategy))
    {
        if (!strategy_) {
            throw std::invalid_argument(
                "Payment strategy cannot be null."
            );
        }
    }

    // Validate the amount and delegate payment behavior
    // to the currently selected strategy.
    // Pass the integer amount by value because copying numeric types is cheap.
    // strategy_->pay(...) is shorthand for (*strategy_).pay(...).
    void process_payment(std::int64_t amount_cents) const
    {
        if (amount_cents <= 0) {
            throw std::invalid_argument(
                "Payment amount must be greater than zero."
            );
        }

        strategy_->pay(amount_cents);
    }

    // Replace the current polymorphic strategy at runtime.
    // Move transfers unique ownership to strategy_; the previously owned
    // strategy is destroyed automatically.
    void set_strategy(
        std::unique_ptr<PaymentStrategy> strategy
    )
    {
        if (!strategy) {
            throw std::invalid_argument(
                "Payment strategy cannot be null."
            );
        }

        strategy_ = std::move(strategy);
    }
};


int main()
{
    try {
        PaymentProcessor processor{
            std::make_unique<CreditCardPayment>()
        };

        processor.process_payment(4999); // 49.99

        processor.set_strategy(
            std::make_unique<PayPalPayment>()
        );

        processor.process_payment(2500); // 25.00

        processor.set_strategy(
            std::make_unique<CryptoPayment>()
        );

        processor.process_payment(10000); // 100.00
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
