// ------------------------------------------------------------
// Architecture overview
// ------------------------------------------------------------
//
// EventType
//  - Defines the supported event categories as strongly typed enum values.
//  - Avoids typo-prone string identifiers for event types.
//
// Event
//  - Represents a single published event.
//  - Owns its EventType and message.
//  - Enforces the invariant that every event contains a non-empty message.
//  - Exposes event data through read-only accessors.
//
// EventListener
//  - Abstract observer interface for objects that react to events.
//  - Defines the polymorphic on_event() operation.
//  - Concrete listeners can remain stateless or maintain their own internal state.
//
// LoggingListener
//  - Reacts to an event by printing its type and message.
//
// EmailListener
//  - Reacts to an event by simulating an email notification.
//  - Uses the event type as the subject and the event message as the body.
//
// MetricsListener
//  - Demonstrates a stateful observer.
//  - Counts how many events it receives.
//
// EventDispatcher
//  - Acts as the central publisher/subject in the Observer pattern.
//  - Owns registered listeners through std::unique_ptr<EventListener>.
//  - Groups listeners by EventType so multiple observers can subscribe
//    to the same event category.
//  - register_listener() transfers listener ownership into the dispatcher.
//  - publish() finds the listeners for an EventType and notifies each one
//    polymorphically through EventListener::on_event().
//  - Publishing an event with no registered listeners is a valid no-op.
//
// Design objectives
//  - Observer Pattern:
//    EventDispatcher publishes events to all observers registered for
//    the corresponding EventType.
//
//  - Polymorphism:
//    Different concrete listeners are stored and invoked through the common
//    EventListener interface.
//
//  - Composition:
//    EventDispatcher gains behavior by owning and coordinating independent
//    listener objects rather than inheriting listener-specific behavior.


#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>


// ------------------------------------------------------------
// Event types
// ------------------------------------------------------------

// Strongly typed identifiers for the supported event categories.
// Note:
//  - Using enum class prevents accidental mixing with unrelated integer values
//    and avoids typo-prone string-based event names.
enum class EventType {
    UserRegistered,
    OrderPlaced,
    PaymentFailed
};

// Convert an EventType to a readable name for logging or display.
// Note: 
//  - Returning std::string_view is safe here because all returned values
//    are string literals with static lifetime.
std::string_view to_string(EventType type) {
    switch (type) {
        case EventType::UserRegistered:
            return "UserRegistered";

        case EventType::OrderPlaced:
            return "OrderPlaced";

        case EventType::PaymentFailed:
            return "PaymentFailed";
    }

    return "Unknown";
}


// ------------------------------------------------------------
// Event
// ------------------------------------------------------------

// Represents a single event that can be published through the event system.
// Notes:
//  - type_ identifies the event category using the strongly typed EventType enum.
//  - message_ stores the event's owned textual payload.
//  - The class is intentionally immutable after construction: no setters are 
//    provided.
//  - validate_message() enforces the invariant that every Event must contain
//    a non-empty message.
class Event {
private:
    EventType type_;
    std::string message_;
    
    // Validate the message before the Event is considered fully constructed.
    // Note:
    //  - static is appropriate because validation depends only on the supplied 
    //    value and does not require access to object state.
    static void validate_message(std::string_view message) {
        if (message.empty()) {
             throw std::invalid_argument("Event message must not be empty!");
        }
    }
public:
    // Construct an Event with a fixed type and message.
    // Notes:
    //  - EventType is copied because it is a small value type.
    //  - message is taken by value because Event stores its own copy.
    //  - std::move transfers the local string's resources into message_.
    //  - Validation is performed on message_ after it has taken ownership
    //    of the supplied message.
    Event(EventType type, std::string message)
        : type_(type),
          message_(std::move(message))
    {
        validate_message(message_);
    }
    
    // Return the event type by value.
    // Note:
    //  - Returning an enum by value is cheap and does not expose internal storage.
    EventType type() const noexcept {
        return type_;
    }
    
    // Provide read-only access to the stored message without copying it.
    // Note:
    //  - The returned reference remains valid only while this Event object exists.
    const std::string& message() const noexcept {
        return message_;
    }
};


// ------------------------------------------------------------
// Listener interface
// ------------------------------------------------------------

// Abstract observer interface for objects that react to published events.
// Notes:
//  - The virtual destructor ensures safe destruction through EventListener pointers.
//  - on_event() is pure virtual, so every concrete listener must define
//    its own response to an Event.
//  - The Event is passed by const reference to avoid copying and to prevent
//    listeners from modifying the published event through this reference.
//  - on_event() itself is intentionally non-const so concrete listeners
//    may update their own internal state while handling an event.
class EventListener {
public:
    virtual ~EventListener() = default;
    
    virtual void on_event(const Event& event) = 0;
};


// ------------------------------------------------------------
// Concrete listeners
// ------------------------------------------------------------

// Concrete listener that logs published events to standard output.
// Notes:
//  - Inherits from EventListener and provides the required on_event() implementation.
//  - The Event is received by const reference, avoiding a copy and preventing
//    modification of the published event through this interface.
//  - to_string() converts the strongly typed EventType into readable text.
//  - The listener is stateless: it reacts to each event without storing data.
//  - override lets the compiler verify that EventListener::on_event() is
//    implemented with the correct signature.
class LoggingListener : public EventListener {
public:
    void on_event(const Event& event) override {
        std::cout 
            << '[' 
            << to_string(event.type()) 
            << "]: " 
            << event.message() 
            << '\n';
    }
};


// Concrete listener that simulates sending an email notification.
// Notes:
//  - Inherits from EventListener and provides the required on_event() implementation.
//  - The published Event is received by const reference, so the listener can read
//    its data without copying or modifying the original event.
//  - The EventType is converted to readable text and used as the email subject.
//  - The Event message is used as the email body.
//  - This listener is stateless and does not store any data between notifications.
//  - override lets the compiler verify that EventListener::on_event() is
//    implemented with the correct signature.
class EmailListener : public EventListener {
public:
    void on_event(const Event& event) override {
        std::cout 
            << "Sending email notification.\n" 
            << "Subject: " 
            << to_string(event.type()) 
            << '\n' 
            << "Body: "
            << event.message()
            << '\n';
    }
};


// Concrete listener that tracks how many events it receives.
// Notes:
//  - Inherits from EventListener and provides the required on_event() implementation.
//  - event_count_ stores the listener's internal state and starts at zero.
//  - on_event() does not need to inspect the Event contents, so the parameter
//    name is intentionally omitted.
//  - Each received event increments the counter by one.
//  - event_count() exposes the current count by value without allowing callers
//    to modify the stored state directly.
//  - The getter is const because it does not modify the listener.
//  - noexcept is appropriate because returning std::size_t cannot throw.
class MetricsListener : public EventListener {
private:
    std::size_t event_count_{0};
    
public:
    void on_event(const Event&) override {
        ++event_count_;
    }
    
    std::size_t event_count() const noexcept {
        return event_count_;
    } 
};


// ------------------------------------------------------------
// Event dispatcher
// ------------------------------------------------------------

// Central dispatcher responsible for registering listeners and publishing events.
// Notes:
//  - listeners_ groups observers by EventType.
//  - Each EventType can have multiple registered EventListener objects.
//  - std::unique_ptr gives EventDispatcher exclusive ownership of every listener.
//  - Listeners are stored through the EventListener interface, enabling runtime
//    polymorphism across different concrete listener types.
//  - register_listener() transfers ownership of a listener into the dispatcher.
//  - publish() looks up listeners for the event type and notifies each one.
//  - Publishing an event with no registered listeners is treated as a valid state
//    and simply results in no action.
class EventDispatcher {
private:
    // Store owned listeners grouped by the EventType they observe.
    std::unordered_map<
        EventType, 
        std::vector<std::unique_ptr<EventListener>>
    > listeners_;
    
    // Ensure that every registration contains an actual listener object.
    // Note:
    //  - The unique_ptr is inspected by const reference so ownership is not 
    //    transferred during validation.
    static void validate_listener(const std::unique_ptr<EventListener>& listener) {
        if (!listener) {
            throw std::invalid_argument("Listener must not be null!");
        }
    }
public:
    // EventDispatcher starts with no registered listeners.
    EventDispatcher() = default;

    // Register a listener for a specific EventType.
    // Notes:
    //  - EventType is passed by value because it is a small enum value.
    //  - listener is passed by value because ownership is transferred to
    //    EventDispatcher.
    //  - operator[] creates an empty vector automatically when this EventType
    //    has no listeners yet.
    //  - std::move transfers ownership of the concrete listener into the vector.
    void register_listener(
        EventType type,
        std::unique_ptr<EventListener> listener
        ) {
            validate_listener(listener); 
            listeners_[type].push_back(std::move(listener));
    }
        
    // Publish an Event to all listeners registered for its EventType.
    // Notes:
    //  - The Event is passed by const reference to avoid copying and to prevent
    //    the dispatcher or listeners from modifying the published Event.
    //  - find() performs a lookup without creating a new map entry.
    //  - If no listeners are registered for the EventType, publishing simply returns.
    //  - Each listener is accessed through the EventListener interface, so the
    //    correct concrete on_event() implementation is selected polymorphically.
    //  - The unique_ptr itself is accessed by const reference because ownership
    //    is not modified while publishing.
    void publish(const Event& event) {
        auto it = listeners_.find(event.type());
        if (it == listeners_.end()) {
            // no listeners registered for this EventType
            return;
        }
        for (const auto& ptr : it->second) {
            ptr->on_event(event);
        }
    }
};


// ------------------------------------------------------------
// Demonstration
// ------------------------------------------------------------

int main() {
    // Create the central event dispatcher.
    // Note:
    //  - It starts with no registered listeners.
    EventDispatcher dispatcher;
    
    // Create the stateful MetricsListener separately so we can keep a
    // non-owning pointer to it before transferring ownership to the dispatcher.
    auto metrics_listener = std::make_unique<MetricsListener>();
    
    // get() returns a raw pointer without transferring ownership.
    // Note:
    //  - The pointer remains valid as long as the dispatcher keeps owning
    //    the same MetricsListener object.
    MetricsListener* metrics_ptr = metrics_listener.get();
    
    // Register two different listeners for UserRegistered.
    // Note:
    //  - Both listeners will be notified whenever that event type is published.
    dispatcher.register_listener(
        EventType::UserRegistered, 
        std::make_unique<LoggingListener>()
    );
    
    dispatcher.register_listener(
        EventType::UserRegistered, 
        std::make_unique<EmailListener>()
    );

    // Register a LoggingListener for OrderPlaced.
    dispatcher.register_listener(
        EventType::OrderPlaced, 
        std::make_unique<LoggingListener>()
    );

    // Transfer ownership of the previously created MetricsListener
    // into the dispatcher for OrderPlaced events.
    dispatcher.register_listener(
        EventType::OrderPlaced, 
        std::move(metrics_listener)
    );
    
    // Create and publish a UserRegistered event.
    // Note:
    //  - Both LoggingListener and EmailListener will react to it.
    Event user_registered_1(
        EventType::UserRegistered, 
        "User Alice was registered."
    );
    
    dispatcher.publish(user_registered_1);
    
    // Create and publish the first OrderPlaced event.
    // Note:
    //  - LoggingListener prints it and MetricsListener increments its counter.
    Event order_placed_1(
        EventType::OrderPlaced, 
        "Order 42 was placed."
    );
    
    dispatcher.publish(order_placed_1);
    
    // Create and publish another OrderPlaced event.
    // Note:
    //  - The MetricsListener counter is incremented a second time.
    Event order_placed_2(
        EventType::OrderPlaced, 
        "Order 43 was placed."
    );
    
    dispatcher.publish(order_placed_2);
    
    // Inspect the stateful listener through the non-owning raw pointer.
    // Note:
    //  - At this point the expected count is 2.
    std::cout 
        << "Current count: " 
        << metrics_ptr->event_count()
        << '\n';
    
    // Create and publish an event type with no registered listeners.
    // Note:
    //  - EventDispatcher treats this as a valid state and simply does nothing.
    Event payment_failed_1(
        EventType::PaymentFailed, 
        "Payment failed."
    );
    
    dispatcher.publish(payment_failed_1);
    
    return 0;
}
