#include <cstdint>
#include <iostream>
#include <stdexcept>

class BankAccount {
private:
    // Store money as integer cents to avoid floating-point rounding issues.
    // Use explicit std:: qualification to avoid importing the entire namespace, 
    // a trailing underscore for the member, and in-class initialization to 
    // start the balance at zero.
    std::int64_t balance_cents_{0};

public:
    // Pass the integer amount by value because copying numeric types is cheap.
    // Validate before modifying the balance to keep the account in a valid state.
    void deposit(std::int64_t amount_cents)
    {
        if (amount_cents <= 0) {
            throw std::invalid_argument(
                "Deposit amount must be greater than zero."
            );
        }

        balance_cents_ += amount_cents;
    }

    // Pass the integer amount by value because copying numeric types is cheap.
    // Validate the amount and available balance before changing the account state.
    void withdraw(std::int64_t amount_cents)
    {
        if (amount_cents <= 0) {
            throw std::invalid_argument(
                "Withdrawal amount must be greater than zero."
            );
        }

        if (amount_cents > balance_cents_) {
            throw std::runtime_error(
                "Insufficient funds."
            );
        }

        balance_cents_ -= amount_cents;
    }

    // Read-only accessor: return by value because integer types are cheap to copy.
    // noexcept is optional, but explicitly guarantees that this getter cannot throw.
    std::int64_t balance() const noexcept
    {
        return balance_cents_;
    }
};

int main()
{
    try {
        BankAccount account;

        account.deposit(10000);   // 100.00
        account.withdraw(2500);   // 25.00

        std::cout << "Balance: "
                  << account.balance() / 100.0
                  << '\n';
    }
    catch (const std::invalid_argument& error) {
        std::cerr << "Invalid input: "
                  << error.what()
                  << '\n';
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Transaction failed: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
