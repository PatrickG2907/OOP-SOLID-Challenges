// ------------------------------------------------------------
// Architecture overview
// ------------------------------------------------------------
//
// FlowContext
//  - Holds the shared state used during one flow execution.
//  - Concrete steps can read from and modify this state.
//  - FlowEngine itself does not need to understand the business data.
//
// FlowStep
//  - Abstract command interface for one executable step.
//  - Concrete steps implement execute() and return an Outcome.
//  - A step performs work, but does not decide which step runs next.
//
// FlowDefinition
//  - Stores the structure and branching rules of the workflow as data.
//  - Maps: current StepId + Outcome -> optional next StepId.
//  - std::nullopt represents the end of the flow.
//
// FlowEngine
//  - Owns the FlowDefinition and the registered FlowStep command objects.
//  - Executes the current command, receives its Outcome, and asks
//    FlowDefinition which step should run next.
//  - Contains no business-specific branching logic.
//  - Concrete commands can be replaced dynamically without changing
//    the engine or the flow definition.


#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>


// Type aliases give domain-specific names to std::string values,
// making step identifiers and outcomes easier to distinguish conceptually.
// They improve readability but do not create distinct compiler-enforced types.
using StepId = std::string;
using Outcome = std::string;

// Type alias for the transition table of one step.
// Maps each possible Outcome to the next StepId;
// std::nullopt represents a terminal transition with no next step.
using OutcomeTransitions = std::unordered_map<Outcome, std::optional<StepId>>;


// ------------------------------------------------------------
// Shared flow data
// ------------------------------------------------------------

// Holds data that can be read or modified by individual flow steps.
// The engine itself does not need to understand this business data.
class FlowContext {
private:
    // Private members hide the internal flow state from outside code.
    // Access and modification happen through public methods, preserving encapsulation.
    // shipped_ uses in-class initialization and starts as false.
    bool order_valid_;
    bool payment_should_succeed_;
    bool shipped_{false};

public:
    // Constructor.
    // Notes:
    //  - Initialize the private flow state through a member initializer list.
    //  - bool parameters are passed by value because they are cheap to copy.
    //  - shipped_ is initialized separately in-class and starts as false.
    FlowContext(
        bool order_valid,
        bool payment_should_succeed
    )
        : order_valid_(order_valid),
          payment_should_succeed_(payment_should_succeed)
    {
    }

    // Return order status: valid/invalid.
    // Notes:
    //  - Return the flag by value because bool is cheap to copy.
    //  - Trailing const means the getter does not modify FlowContext.
    //  - noexcept guarantees that this getter does not throw.
    bool order_valid() const noexcept
    {
        return order_valid_;
    }

    // Return payment status: successful/failed.
    // Notes:
    //  - Return the flag by value because bool is cheap to copy.
    //  - Trailing const means the getter does not modify FlowContext.
    //  - noexcept guarantees that this getter does not throw.
    bool payment_should_succeed() const noexcept
    {
        return payment_should_succeed_;
    }

    // Modify shipping status.
    // Notes:
    //  - The internal shipped_ state gets modified, so this method cannot be const.
    //  - noexcept is appropriate because assigning a bool cannot throw an exception.
    void mark_shipped() noexcept
    {
        shipped_ = true;
    }

    // Return shipping status: shipped/not shipped.
    // Notes:
    //  - Return the flag by value because bool is cheap to copy.
    //  - Trailing const means the getter does not modify FlowContext.
    //  - noexcept guarantees that this getter does not throw.
    bool shipped() const noexcept
    {
        return shipped_;
    }
};


// ------------------------------------------------------------
// Command interface
// ------------------------------------------------------------

// Abstract command interface for one executable flow step.
// Each command performs one unit of work and returns an outcome.
// It deliberately does not decide which step should execute next.
// Notes:
//  - Outcome is a type alias for std::string, used to express the step result.
//  - virtual + = 0 makes execute() pure virtual, so concrete steps must implement it
//    and FlowStep itself cannot be instantiated.
//  - FlowContext is passed by reference to avoid copying and to allow the step
//    to modify the shared flow state.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class FlowStep {
public:
    virtual ~FlowStep() = default;

    virtual Outcome execute(
        FlowContext& context
    ) = 0;
};


// ------------------------------------------------------------
// Concrete commands
// ------------------------------------------------------------

// Concrete command that validates the order using the shared FlowContext.
// Notes:
//  - override verifies the FlowStep interface is implemented correctly.
//  - The step returns an Outcome value ("valid" or "invalid") rather than
//    deciding which step runs next; branching is handled by FlowDefinition.
class ValidateOrderStep : public FlowStep {
public:
    Outcome execute(
        FlowContext& context
    ) override
    {
        std::cout << "Validating order...\n";

        if (context.order_valid()) {
            std::cout << "Order is valid.\n";
            return "valid";
        }

        std::cout << "Order is invalid.\n";
        return "invalid";
    }
};

// Concrete command that processes payment using data from the shared FlowContext.
// Notes:
//  - override verifies that the FlowStep interface is implemented correctly.
//  - The step returns "success" or "failure" as an Outcome rather than choosing
//    the next step itself; FlowDefinition handles that branching.
class PaymentStep : public FlowStep {
public:
    Outcome execute(
        FlowContext& context
    ) override
    {
        std::cout << "Processing payment...\n";

        if (context.payment_should_succeed()) {
            std::cout << "Payment succeeded.\n";
            return "success";
        }

        std::cout << "Payment failed.\n";
        return "failure";
    }
};

// Concrete command that updates the shared FlowContext by marking the order as shipped.
// Notes:
//  - FlowContext is passed by non-const reference because this step modifies shared 
//    flow state.
//  - override verifies that the FlowStep interface is implemented correctly.
//  - The step returns the Outcome "done"; FlowDefinition decides what happens next.
class ShippingStep : public FlowStep {
public:
    Outcome execute(
        FlowContext& context
    ) override
    {
        std::cout << "Shipping order...\n";

        context.mark_shipped();

        return "done";
    }
};

// Concrete command that rejects the order and returns the Outcome "done".
// Notes:
//  - The FlowContext parameter is intentionally unnamed because this step does not
//    use it.
//  - override verifies that the FlowStep interface is implemented correctly.
//  - As with the other commands, FlowDefinition decides what happens next.
class RejectOrderStep : public FlowStep {
public:
    Outcome execute(
        FlowContext&
    ) override
    {
        std::cout << "Order rejected.\n";

        return "done";
    }
};


// Concrete replacement command that always returns the Outcome "failure".
// Notes:
//  - The FlowContext parameter is intentionally unnamed because this step does not 
//    use it.
//  - This demonstrates that a command can be swapped without changing FlowEngine.
//  - override verifies that the FlowStep interface is implemented correctly.
class AlwaysFailPaymentStep : public FlowStep {
public:
    Outcome execute(
        FlowContext&
    ) override
    {
        std::cout << "Replacement payment step: payment failed.\n";

        return "failure";
    }
};


// ------------------------------------------------------------
// Flow definition
// ------------------------------------------------------------

// Stores the branching rules as data.
//
// A transition consists of:
//     current step + outcome -> next step
//
// std::nullopt represents the end of the flow.
class FlowDefinition {
private:
    StepId start_step_;
    
    // transitions_ is a nested unordered_map:
    //  - outer (left) key: current StepId
    //  - outer value: OutcomeTransitions
    //  - OutcomeTransitions maps each possible Outcome to the next StepId
    //  - std::nullopt means the flow ends after that outcome.
    std::unordered_map<StepId, OutcomeTransitions> transitions_;

public:
    // Constructor.
    // Notes:
    //  - Construct the flow with its initial StepId.
    //  - explicit prevents unintended implicit conversion from StepId to FlowDefinition.
    //  - Take start_step by value because the class stores its own copy, then move the
    //    local parameter into start_step_ to avoid an additional string copy.
    //  - The member initializer list initializes start_step_ directly.
    //  - Validation ensures every FlowDefinition starts with a non-empty start step.
    //  - transitions_ is default-constructed as an empty unordered_map.
    //  - Transitions are added later through add_transition().
    explicit FlowDefinition(StepId start_step)
        : start_step_(std::move(start_step))
    {
        if (start_step_.empty()) {
            throw std::invalid_argument(
                "Start step cannot be empty."
            );
        }
    }

    // Return ID of start step.
    // Notes:
    //  - Return start_step_ by const reference to avoid copying the string
    //    and prevent modification through the returned reference.
    //  - Trailing const means the getter does not modify FlowDefinition.
    //  - noexcept is appropriate because returning the member reference cannot throw.
    const StepId& start_step() const noexcept
    {
        return start_step_;
    }

    // Add transition.
    // Notes:
    //  - Add one transition rule to the initially empty transition map.
    //  - Each entry maps: current StepId + Outcome -> next StepId.
    //  - Repeated calls gradually build the complete flow definition.
    //  - Take transition data by value because FlowDefinition stores its own values.
    //  - The local parameters can then be moved into the transition map,
    //    avoiding additional copies when the function stores them.
    void add_transition(
        StepId from,
        Outcome outcome,
        std::optional<StepId> to
    )
    {
        // Validate transition input.
        // Notes:
        //  - Source step and outcome must not be empty.
        //  - Destination is optional because std::nullopt represents the end of the flow;
        //    if a destination is present, its StepId must not be empty.
        if (from.empty()) {
            throw std::invalid_argument(
                "Source step cannot be empty."
            );
        }

        if (outcome.empty()) {
            throw std::invalid_argument(
                "Outcome cannot be empty."
            );
        }

        if (to && to->empty()) {
            throw std::invalid_argument(
                "Destination step cannot be empty."
            );
        }

        // Find the transition map for the source step, or create an empty one
        // if the source step does not exist yet.
        // Notes:
        //  - Store a reference to that nested map so it can be modified directly.
        //  - Move from because the local StepId is no longer needed afterward.
        auto& step_transitions = transitions_[std::move(from)];

        // Try to insert the Outcome -> destination transition into the nested map.
        // Notes:
        //  - emplace returns a pair containing an iterator to the entry and a bool
        //    indicating whether a new entry was inserted.
        //  - Move outcome and to because the local values are no longer needed afterward.
        const auto [iterator, inserted] =
            step_transitions.emplace(
                std::move(outcome),
                std::move(to)
            );

        // Reject duplicate transitions for the same source step and Outcome:
        // If emplace did not insert a new entry, that Outcome already exists
        // in the source step's transition map.
        if (!inserted) {
            throw std::invalid_argument("Transition already exists.");
        }
    }

    // Determine the next step in the flow based on the current StepId
    // and the Outcome produced by that step.
    // Notes:
    //  - Take current_step and outcome by const reference to avoid copying
    //    while keeping them read-only.
    //  - Trailing const means this lookup does not modify FlowDefinition.
    std::optional<StepId> next_step(
        const StepId& current_step,
        const Outcome& outcome
    ) const
    {
        // Look up the transition table for the current step.
        // Notes:
        //  - Find the entry for the current StepId in transitions_.
        //  - The iterator points to a (StepId, OutcomeTransitions) pair.
        //  - find() is used instead of operator[] because this is a read-only lookup
        //    and we do not want to create a new entry when the step is missing.
        const auto step_iterator = transitions_.find(current_step);

        // If the current step has no configured transitions,
        // the flow definition is incomplete or invalid for this execution path.
        if (step_iterator == transitions_.end()) {
            throw std::runtime_error("No transitions configured for step: " + current_step);
        }

        // This lookup finds the transition that corresponds to the Outcome
        // produced by the current step.
        // Notes:
        //  - step_iterator points to a StepId -> OutcomeTransitions entry.
        //  - step_iterator->second is the nested OutcomeTransitions unordered map.
        //  - outcome_iterator then points to an Outcome -> optional<StepId> entry.
        const auto outcome_iterator = step_iterator->second.find(outcome);

        // Check whether the requested Outcome exists in the current step's
        // OutcomeTransitions map.
        // Note:
        //  - end() means find() did not locate a matching transition,
        //    so the flow has no configured next step for this Outcome.
        if (
            outcome_iterator == step_iterator->second.end()) {
            throw std::runtime_error("No transition configured for outcome: " + outcome);
        }
        
        // Return the configured destination for the matched Outcome.
        // Notes:
        //  - outcome_iterator->second is an std::optional<StepId>:
        //  - it contains the next step when the flow continues,
        //    or std::nullopt when this transition ends the flow.
        return outcome_iterator->second;
    }
};


// ------------------------------------------------------------
// Flow engine
// ------------------------------------------------------------

// Generic execution engine.
// Notes:
//  - FlowEngine owns concrete command objects through FlowStep,
//    while branching behavior comes from FlowDefinition.
//  - The engine contains no business-specific if/else branching (Open/Closed Principle).
class FlowEngine {
private:
    // Own the flow definition as part of the engine's configuration.
    // Storing it by value gives FlowEngine an independent lifetime and avoids
    // depending on an external FlowDefinition object remaining alive.
    FlowDefinition definition_;

    // Own the concrete flow-step commands, indexed by their StepId.
    // Notes:
    //  - unique_ptr gives FlowEngine exclusive ownership of each polymorphic FlowStep
    //    and ensures the command objects are destroyed automatically.
    //  - The map allows the engine to look up and dynamically replace steps by ID.
    std::unordered_map<StepId, std::unique_ptr<FlowStep>> steps_;

public:
    // Constructor.
    // Notes:
    //  - Take the definition by value because FlowEngine stores it as an owned member,
    //    then move the local parameter into definition_ to avoid an additional copy.
    //  - explicit prevents unintended implicit conversion from FlowDefinition to 
    //    FlowEngine.
    explicit FlowEngine(
        FlowDefinition definition
    )
        : definition_(std::move(definition))
    {
    }

    // Add or replace a command under a given StepId.
    // Notes:
    //  - Take id by value because it is stored in the map and can be moved into it.
    //  - set_step() uses runtime polymorphism: any class derived from FlowStep
    //    can be registered and executed through the FlowStep interface.
    //  - Polymorphism requires access through a pointer or reference.
    //  - A unique_ptr is used here because FlowEngine owns the concrete step objects
    //    and needs to be able to replace them dynamically.
    //  - Validate that the ID is non-empty and that the pointer is not null.
    //  - insert_or_assign() inserts a new command or replaces the existing one;
    //  - replacing a unique_ptr automatically destroys the previously owned command.
    void set_step(
        StepId id,
        std::unique_ptr<FlowStep> step
    )
    {
        if (id.empty()) {
            throw std::invalid_argument("Step ID cannot be empty.");
        }

        if (!step) {
            throw std::invalid_argument("Flow step cannot be null.");
        }

        steps_.insert_or_assign(std::move(id), std::move(step));
    }

    // Execute the flow starting from the configured start step.
    // Note:
    //  - FlowContext is passed by non-const reference because individual commands
    //    may modify the shared flow state during execution.
    void run(
        FlowContext& context
    )
    {
        // Copy the configured start StepId because current_step changes
        // as execution moves through the flow.
        StepId current_step = definition_.start_step();

        while (true) {
            // Look up the command registered for the current StepId.
            // Note:
            //  - find() is used because this is a lookup and should not create entries.
            const auto step_iterator = steps_.find(current_step);

              // The flow cannot continue if no command is registered
             // for the current step.
            if (step_iterator == steps_.end()) {
                throw std::runtime_error("No command registered for step: " + current_step);
            }

            // Command Pattern:
            // execute the concrete command through the FlowStep abstraction.
            // step_iterator->second is the unique_ptr<FlowStep> owned by the engine.
            const Outcome outcome = step_iterator->second->execute(context);

            // Data-driven branching:
            // ask FlowDefinition which step follows for this StepId + Outcome,
            // instead of hard-coding business-specific if/else branches here.
            const std::optional<StepId> next =
                definition_.next_step(
                    current_step,
                    outcome
                );

            // std::nullopt represents a terminal transition,
            // so execution ends when there is no next StepId.
            if (!next) {
                break;
            }
            // Dereference the optional to access its contained StepId.
            // Note:
            //  - At this point next is known to contain a value because
            //    the std::nullopt case was handled above.
            current_step = *next;
        }
    }
};


// ------------------------------------------------------------
// Example
// ------------------------------------------------------------

int main()
{
    try {
        // Define the flow and set "validate" as the StepId of the first step to execute.
        FlowDefinition definition{"validate"};

        // Add all valid transitions for this flow.
        // Each rule maps: current StepId + Outcome -> next StepId.
        // std::nullopt marks a terminal transition and ends the flow.
        definition.add_transition("validate", "valid", "payment");
        definition.add_transition("validate", "invalid", "reject");
        definition.add_transition("payment", "success", "shipping");
        definition.add_transition("payment", "failure", "reject");
        definition.add_transition("shipping", "done", std::nullopt);
        definition.add_transition("reject", "done", std::nullopt);

        // Create the FlowEngine and transfer the configured FlowDefinition into it.
        // Note:
        //  - std::move allows the definition to be moved instead of copied.
        FlowEngine engine{std::move(definition)};

        // Compose the engine from concrete command objects.
        // Notes:
        //  - Each derived FlowStep is created with make_unique and registered
        //    under the StepId used by the flow definition.
        //  - FlowEngine takes exclusive ownership of the commands through unique_ptr.
        engine.set_step("validate", std::make_unique<ValidateOrderStep>());
        engine.set_step("payment", std::make_unique<PaymentStep>());
        engine.set_step("shipping", std::make_unique<ShippingStep>());
        engine.set_step("reject", std::make_unique<RejectOrderStep>());

        std::cout << "=== First flow ===\n";

        // Create the shared flow state for the first execution.
        // Notes:
        //  - The order is valid and payment is configured to succeed.
        //  - shipped_ starts as false until ShippingStep marks it as shipped.
        FlowContext first_context{true, true};

        // Run the configured flow using first_context as the shared execution state.
        // The context is passed by reference so each command sees and may modify
        // the same FlowContext object throughout the flow.
        engine.run(first_context);

        std::cout << "Shipped: "
                  << std::boolalpha
                  << first_context.shipped()
                  << "\n\n";

        // Dynamically replace the command registered under the "payment" StepId.
        // Notes:
        //  - The old PaymentStep is destroyed automatically when its unique_ptr 
        //    is replaced.
        //  - The flow definition itself does not change; only the behavior executed for
        //    the "payment" step is swapped to AlwaysFailPaymentStep.
        engine.set_step("payment", std::make_unique<AlwaysFailPaymentStep>());

        std::cout << "=== Second flow ===\n";

        // Create the shared flow state for the second execution.
        // Notes:
        //  - The order is valid and payment is configured to succeed.
        //  - shipped_ starts as false until ShippingStep marks it as shipped.
        FlowContext second_context{true, true};

        // Run the configured flow using second_context as the shared execution state.
        // The context is passed by reference so each command sees and may modify
        // the same FlowContext object throughout the flow.
        engine.run(second_context);

        std::cout << "Shipped: "
                  << std::boolalpha
                  << second_context.shipped()
                  << '\n';
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
