# OOP & SOLID Challenges (Expert Level)

## Challenge 1: Flow Engine

### Task
Create a flow engine that executes steps in a predefined order. Each step can be swapped out dynamically without changing the engine.

### Objectives
- **Command Pattern**
- **Composition over Inheritance**
- **Data-driven branching**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - Each step (`StepA`, `StepB`, etc.) contains its own logic; the engine doesn’t know the details.
  
- **Polymorphism**:  
    - The engine (`FlowEngine`) only knows about the `Step` interface, so any step can be swapped in without changing the engine.

- **Abstraction**:  
    - `Step` defines a common interface (`execute()`) that all steps follow, hiding implementation details.

- **Composition over Inheritance**:  
    - The engine orchestrates steps by holding references to them rather than inheriting their behavior.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `FlowEngine` only manages the flow, while the individual steps handle their own logic.

- **OCP (Open/Closed Principle)**:  
    - New step types or branching rules can be added without modifying existing engine code.

- **LSP (Liskov Substitution Principle)**:  
    - Any class implementing `Step` can replace another, and the engine still works correctly.

- **ISP (Interface Segregation Principle)**:  
    - The `Step` interface is minimal — just `execute()` — so steps aren’t forced to implement unrelated methods.

- **DIP (Dependency Inversion Principle)**:  
    - `FlowEngine` depends on the `Step` abstraction, not concrete implementations.

**Design Patterns**
- **Command Pattern**:  
    - Each step is a command encapsulating an action.
  
- **Data-driven branching**:  
    - Steps return a result key, and the engine maps it to the next step, keeping the flow engine generic.

---

## Challenge 2: Plugin System

### Task
Create a system that supports a core application with interchangeable plugins.

### Objectives
- **Composition over Inheritance**
- **Open/Closed Principle**
- **Interface Segregation**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - Defined a `Plugin` interface (abstract base class) that represents what a plugin does, not how it does it.

- **Encapsulation**:  
    - Each plugin contains its own behavior and hides its internal logic from the core app.

- **Polymorphism**:  
    - The core app treats all plugins the same and calls `run()` without knowing the concrete class.

- **Composition over Inheritance**:  
    - The application has plugins instead of being extended by them.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - The application only coordinates plugins.
    - Each plugin implements one feature.

- **OCP (Open/Closed Principle)**:  
    - New features are added by creating new plugins.  
    - No modification to the core application is required.

- **LSP (Liskov Substitution Principle)**:  
    - Any plugin can replace another as long as it implements the `Plugin` interface, and the app behaves correctly regardless of the plugin used.

- **ISP (Interface Segregation Principle)**:  
    - The `Plugin` interface is small and focused.
    - Plugins are not forced to implement methods they don’t need.

- **DIP (Dependency Inversion Principle)**:  
    - The core app depends on the `Plugin` abstraction, not concrete plugin implementations.
    - High-level logic does not depend on low-level details.

---

## Challenge 3: Event-Driven System

### Task
Build an event-driven system where events are triggered and listeners respond.

### Objectives
- **Observer Pattern**
- **Polymorphism**
- **Composition**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - Event data (`username`, `order_id`, `reason`) is stored in the objects themselves.
    - Validation is done inside the `__init__` method of each event subclass, keeping state integrity.

- **Inheritance & Polymorphism**:  
    - `Event` is an abstract base class (ABC) that all events inherit from.
    - `EventListener` is also an abstract base class that all listeners inherit from.
    - Polymorphism is used in `EventManager.notify()`, where any `EventListener` can handle any `Event`.

- **Composition**:  
    - `EventManager` contains a list of listeners and delegates event handling to them, separating the event system logic from the listener implementation.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Event` subclasses store and validate event data.  
    - `EventManager` manages listener registration and notification.  
    - `Listeners` handle a specific response to events.

- **OCP (Open/Closed Principle)**:  
    - The system is open for extension but closed for modification.  
    - New events or listeners can be added without modifying existing code.

- **LSP (Liskov Substitution Principle)**:  
    - `Event` subclasses can be used anywhere an `Event` is expected.
    - `EventListener` subclasses can be used anywhere an `EventListener` is expected.

- **ISP (Interface Segregation Principle)**:  
    - `EventListener` is a small, focused interface.

- **DIP (Dependency Inversion Principle)**:  
    - `EventManager` depends on the abstraction `EventListener`, not concrete listener classes, allowing loose coupling.

**Design Patterns**
- **Observer Pattern**:  
    - `EventManager` acts as the subject, and listeners act as observers.  
    - Multiple listeners can subscribe to the same event and respond differently.  
    - Loose coupling is maintained because the manager doesn’t know listener internals.

---

## Challenge 4: Billing System

### Task
Create a billing system that generates invoices based on customers and pricing models.

### Objectives
- **Strategy Pattern**
- **Factory Pattern**
- **Polymorphism**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - `Invoice`, `Customer`, and `PricingModel` encapsulate both data and behavior, keeping responsibilities clear.

- **Abstraction**:  
    - `Customer` and `PricingModel` define abstract base classes; subclasses implement specific behavior like `get_discount()` and `calculate()`.

- **Polymorphism**:  
    - `BillingService` uses `customer.get_discount()` and `pricing_model.calculate()` without needing to know the concrete classes.

- **Composition**:  
    - `BillingService` contains `Customer` and `PricingModel` objects instead of inheriting from them.

- **Strategy Pattern**:  
    - `PricingModel` subclasses provide interchangeable pricing strategies.

- **Factory Pattern**:  
    - `CustomerFactory` and `PricingFactory` handle object creation, decoupling client code from concrete classes.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Invoice` stores data.  
    - `Customer` handles discounts.  
    - `PricingModel` handles cost calculation.  
    - `BillingService` generates invoices.

- **OCP (Open/Closed Principle)**:  
    - The system can be extended by adding new customer types or pricing models without modifying existing classes.

- **LSP (Liskov Substitution Principle)**:  
    - Subclasses like `PremiumCustomer` or `UsageBasedPricing` can replace their base classes anywhere without breaking functionality.

- **ISP (Interface Segregation Principle)**:  
    - Abstract classes define minimal, focused interfaces that each subclass implements; no unnecessary methods are forced on them.

- **DIP (Dependency Inversion Principle)**:  
    - `BillingService` depends on abstractions (`Customer` and `PricingModel`) instead of concrete classes, allowing flexible composition.

---

## Challenge 5: Loan Approval System

### Task
Create a loan approval system that evaluates applicants using various rules.

### Objectives
- **Specification Pattern**
- **Polymorphism**
- **Composition**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - The `Rule` abstract base class defines a clear interface (`is_satisfied()`) that all concrete rules must implement.

- **Encapsulation**:  
    - Each rule (`LoanAmountRule`, `CreditScoreRule`, `FraudCheckRule`) encapsulates its own validation logic and business logic.

- **Polymorphism**:  
    - `AndSpecification` and `LoanApprovalService` can operate on any object that implements `Rule`, treating all rules uniformly.

- **Composition**:  
    - `AndSpecification` composes multiple rules to create complex business logic without modifying existing rules.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - Each rule is responsible for validating and checking a single aspect of the applicant (`loan amount`, `credit score`, `fraud`).  
    - The `Applicant` dataclass handles data structure and validation.  
    - `LoanApprovalService` is only responsible for orchestrating the approval decision.

- **OCP (Open/Closed Principle)**:  
    - Rules can be extended or added (e.g., `EmploymentStatusRule`) without modifying existing classes.  
    - `AndSpecification` allows combining rules in new ways without changing rule logic.

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `Rule` can be used wherever a `Rule` is expected, e.g., in `AndSpecification` or `LoanApprovalService`, without breaking the system.

- **ISP (Interface Segregation Principle)**:  
    - The `Rule` interface is minimal, only requiring `is_satisfied()`. No rule is forced to implement unrelated methods.

- **DIP (Dependency Inversion Principle)**:  
    - `LoanApprovalService` depends on the `Rule` abstraction, not on concrete rule implementations.  
    - Rules can be injected dynamically, making the service decoupled and testable.

**Additional OOP/SOLID Practices**
- **Validation in `__post_init__`**:  
    - Both `Applicant` and concrete rules validate their state immediately after creation, ensuring data integrity and enforcing “fail-fast” behavior.

- **Testability**:  
    - Each rule and specification can be tested independently from the service, making unit testing straightforward.

- **Extensibility**:  
    - Adding new rules or combining them in different specifications does not require touching the existing classes, fully embracing open/closed design.

---
