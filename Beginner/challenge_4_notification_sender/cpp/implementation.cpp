#include <iostream>
#include <string>

class NotificationSender {
public:
    // Virtual destructor ensures derived objects are destroyed correctly
    // when deleted through a NotificationSender base-class pointer.
    virtual ~NotificationSender() = default;

    // virtual enables polymorphic overriding.
    // const& avoids copying the strings and prevents modifying them.
    // trailing const means send() does not modify the sender object.
    // = 0 makes the function pure virtual, so concrete derived classes must 
    // implement it.
    virtual void send(
        const std::string& recipient,
        const std::string& message
    ) const = 0;
};

// EmailSender publicly derives from NotificationSender.
// Notes:
//  - Public inheritance allows EmailSender to be used as a NotificationSender.
//  - const& avoids copying the strings and prevents modifying the arguments.
//  - override verifies that send() implements the base-class virtual method.
//  - trailing const means send() does not modify the EmailSender object.
class EmailSender : public NotificationSender {
public:
    void send(
        const std::string& recipient,
        const std::string& message
    ) const override
    {
        std::cout << "Sending email to "
                  << recipient
                  << ": "
                  << message
                  << '\n';
    }
};

// SmsSender publicly derives from NotificationSender.
// Notes:
//  - Public inheritance allows SmsSender to be used as a NotificationSender.
//  - const& avoids copying the strings and prevents modifying the arguments.
//  - override verifies that send() implements the base-class virtual method.
//  - trailing const means send() does not modify the SmsSender object.
class SmsSender : public NotificationSender {
public:
    void send(
        const std::string& recipient,
        const std::string& message
    ) const override
    {
        std::cout << "Sending SMS to "
                  << recipient
                  << ": "
                  << message
                  << '\n';
    }
};

// Runtime polymorphism: depend only on the NotificationSender abstraction.
// The correct send() implementation is selected at runtime.
// const& avoids unnecessary copies and prevents modification.
void send_notification(
    const NotificationSender& sender,
    const std::string& recipient,
    const std::string& message
)
{
    sender.send(recipient, message);
}

int main()
{
    EmailSender email_sender;
    SmsSender sms_sender;

    send_notification(
        email_sender,
        "alice@example.com",
        "Your order has been shipped."
    );

    send_notification(
        sms_sender,
        "+49123456789",
        "Your order has been shipped."
    );

    return 0;
}
