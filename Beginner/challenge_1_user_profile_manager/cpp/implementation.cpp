#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

class User {
    
private:
    // Notes:
    //  - Explicit std:: prefix instead of `using namespace std;`
    //    to avoid namespace pollution and potential naming conflicts.
    //  - A trailing underscore is used for member variables (private here)
    //    to distinguish them clearly from parameters and local variables.
    std::string name_;
    std::string email_;
    int age_;
    
    // Pass by const reference (const &) to avoid copying the string
    // while preventing the function from modifying it.
    static void validate_name(const std::string& name)
    {
        if (name.empty()) {
            throw std::invalid_argument("Name cannot be empty.");
        }
    }

    // Pass by const reference (const &) to avoid copying the string
    // while preventing the function from modifying it.
    static void validate_email(const std::string& email)
    {
        const auto at = email.find('@');

        if (at == std::string::npos || at == 0 || at == email.size() - 1) {
            throw std::invalid_argument("Invalid email address.");
        }
    }

    // Pass int by value because copying small primitive types is cheap.
    static void validate_age(int age)
    {
        if (age < 0) {
            throw std::invalid_argument("Age cannot be negative.");
        }
    }
    
public:
    // Class constructor.
    // Initialize members directly with a member initializer list,
    // then validate them to ensure the object starts in a valid state.
    User(std::string name, std::string email, int age)
        : name_(std::move(name)),
          email_(std::move(email)),
          age_(age)
    {
        validate_name(name_);
        validate_email(email_);
        validate_age(age_);
    }

    // Read-only accessor: return by const reference to avoid copying.
    // noexcept because accessing the member cannot fail.
    const std::string& name() const noexcept
    {
        return name_;
    }

    // Read-only accessor: return by const reference to avoid copying.
    // noexcept because accessing the member cannot fail.
    const std::string& email() const noexcept
    {
        return email_;
    }
    
    // Return int by value because small primitive types are cheap to copy.
    int age() const noexcept
    {
        return age_;
    }

    // Public setter: Validate first, then move the string into the member
    // to avoid an unnecessary copy.
    void set_name(std::string name)
    {
        validate_name(name);
        name_ = std::move(name);
    }

    // Public setter: Validate first, then move the string into the member
    // to avoid an unnecessary copy.
    void set_email(std::string email)
    {
        validate_email(email);
        email_ = std::move(email);
    }
    
    // Copy the int directly because small primitive types are cheap to copy.
    void set_age(int age)
    {
        validate_age(age);
        age_ = age;
    }
};

int main()
{
    // Wrap operations that may throw validation exceptions
    // so invalid input can be handled gracefully.
    try {
        User user{"Alice Smith", "alice@example.com", 30};

        std::cout << "User:\n";
        std::cout << "Name: " << user.name() << '\n';
        std::cout << "Email: " << user.email() << '\n';
        std::cout << "Age: " << user.age() << "\n\n";

        user.set_name("Alice Johnson");
        user.set_email("alice.johnson@example.com");
        user.set_age(31);

        std::cout << "Updated user:\n";
        std::cout << "Name: " << user.name() << '\n';
        std::cout << "Email: " << user.email() << '\n';
        std::cout << "Age: " << user.age() << '\n';
    }
    catch (const std::invalid_argument& error) {
        std::cerr << "Error: " << error.what() << '\n';
    }

    return 0;
}
