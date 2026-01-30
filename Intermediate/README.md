# OOP & SOLID Challenges (Intermediate Level)

## Challenge 1: Payment Processing System

### Task
Support payments via:
- Credit Card
- PayPal
- Crypto

### Objectives
- **Open/Closed Principle**
- **Dependency Inversion**
- **Strategy Pattern**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - Abstract the `PaymentMethod` interface so that all payment types can be used interchangeably.
  
- **Polymorphism**:  
    - `PaymentProcessor` calls `pay()` without knowing the concrete payment method.
  
- **Composition**:  
    - `PaymentProcessor` receives a `PaymentMethod` instead of inheriting from it, making the system more flexible and decoupled.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - Each class has one responsibility: processing, paying, and managing result data.
  
- **OCP (Open/Closed Principle)**:  
    - New payment methods can be added without changing existing code.
  
- **LSP (Liskov Substitution Principle)**:  
    - All payment methods behave consistently and are interchangeable.
  
- **ISP (Interface Segregation Principle)**:  
    - The `PaymentMethod` interface is small and contains only one method: `pay()`.
  
- **DIP (Dependency Inversion Principle)**:  
    - High-level logic depends on the `PaymentMethod` abstraction, not on the concrete classes (`CreditCard`, `PayPal`, `Crypto`).

---

## Challenge 2: Employee Salary Calculator

### Task
Create different employee types:
- Full-time
- Part-time
- Contractor

Each calculates salary differently.

### Objectives
- **Liskov Substitution Principle**
- **Polymorphism**
- **Avoid Type-Checking**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - `PayPolicy` defines what salary calculation is, not how it is performed.  
    - Consumers don’t care about the concrete pay type (e.g., `FullTimePay`, `PartTimePay`).
  
- **Encapsulation**:  
    - Pay-related data (`_monthly_salary`, `_hourly_rate`, `_bonus`) is hidden inside the respective pay classes.
    - Validation and behavior live with the data they operate on, ensuring proper management of pay rules.

- **Polymorphism**:  
    - Different pay strategies (`FullTimePay`, `PartTimePay`, etc.) implement the same interface, allowing the same method call to work for all pay types.

- **Composition**:  
    - `Employee` has a `PayPolicy` instead of being a type of `PayPolicy`. This avoids tight coupling and rigid class hierarchies.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Employee` handles employee identity, while `PayPolicy` handles salary calculation.  
    - Each class has one reason to change.

- **OCP (Open/Closed Principle)**:  
    - New pay types can be added by creating new `PayPolicy` classes. Existing code does not need to change.
  
- **LSP (Liskov Substitution Principle)**:  
    - Any `PayPolicy` subclass can replace another without breaking behavior, thanks to the contract defined by the abstract base class.
  
- **ISP (Interface Segregation Principle)**:  
    - `PayPolicy` exposes only the `calculate_salary()` method, ensuring classes aren’t forced to implement unnecessary functionality.
  
- **DIP (Dependency Inversion Principle)**:  
    - `Employee` depends on the `PayPolicy` abstraction, not concrete pay classes. High-level logic is decoupled from low-level implementation.

---

## Challenge 3: Logging Framework

### Task
Log messages to:
- Console
- File
- Remote server

### Objectives
- **Interface Segregation**
- **Dependency Injection**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - `LoggerStrategy` defines what a logger does (logging messages), not how it is done.  
    - Concrete loggers (e.g., `ConsoleLogger`, `FileLogger`, `RemoteLogger`) hide their implementation details behind a common interface.
  
- **Encapsulation**:  
    - Each concrete logger encapsulates its own logging behavior (e.g., file handling, printing to console, or remote sending).
  
- **Polymorphism**:  
    - `Logger` treats all loggers uniformly.  
    - Different implementations respond to the same `log()` method call in their own specific way.

- **Composition over Inheritance**:  
    - `Logger` is composed of `LoggerStrategy` objects, rather than inheriting from a base class.  
    - This allows dynamic behavior composition at runtime.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Logger` handles validation and orchestration, while concrete loggers handle output.
  
- **OCP (Open/Closed Principle)**:  
    - New logging targets (e.g., `EmailLogger`) can be added without modifying existing code.  
    - Simply create a new `LoggerStrategy` implementation.

- **LSP (Liskov Substitution Principle)**:  
    - `Logger` relies only on the `LoggerStrategy` interface. Any subclass of `LoggerStrategy` can replace another without breaking behavior.

- **ISP (Interface Segregation Principle)**:  
    - `LoggerStrategy` exposes only one method: `log()`. Clients don’t have to implement or depend on unnecessary methods.

- **DIP (Dependency Inversion Principle)**:  
    - Concrete loggers are injected into `Logger` rather than being created internally.  
    - This allows `Logger` to depend on abstractions (`LoggerStrategy`), not on concrete implementations.

---

## Challenge 4: Smart Home Devices

### Task
Devices include:
- Light
- Fan

Each can turn on/off, some can set values.

### Objectives
- **Interface Segregation**
- **Avoid Forcing Unused Methods**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - State (`_state`, `_value`) is private inside behavior classes.  
    - Access is controlled via methods and read-only properties, ensuring no direct manipulation of internal state.

- **Abstraction**:  
    - `Switchable` and `Adjustable` are interfaces that define what devices can do, not how they perform actions.  
    - Composition is used to inject behaviors (e.g., `OnOffBehavior`, `ValueBehavior`) into devices.

- **Polymorphism**:  
    - Functions operate on interfaces (e.g., `Switchable`).  
    - Any device implementing the `Switchable` interface can be used interchangeably.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `OnOffBehavior` handles power state management, `ValueBehavior` manages numerical values and validation, and `Device` orchestrates the device's behavior.
  
- **OCP (Open/Closed Principle)**:  
    - New devices or behaviors can be added without modifying existing classes.  
    - For example, a new device (e.g., `Thermostat`) can be added by creating a new behavior class (e.g., `TemperatureBehavior`).

- **LSP (Liskov Substitution Principle)**:  
    - All `Switchable` devices behave consistently.  
    - Any implementation of `Switchable` can replace another without breaking client code.

- **ISP (Interface Segregation Principle)**:  
    - Interfaces are kept small and focused (e.g., `Switchable`, `Adjustable`).  
    - Devices implement only the methods they actually need.

- **DIP (Dependency Inversion Principle)**:  
    - Devices depend on abstractions (e.g., `Switchable`, `Adjustable` behaviors) rather than concrete implementations.

---

## Challenge 5: Online Order System

### Task
Orders:
- Have items
- Calculate total price
- Apply discounts

### Objectives
- **SRP (Single Responsibility Principle)**
- **Open/Closed Principle**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - `Item` hides its internal `_items` list and exposes methods (`add_item`, `remove_item`, `total`) to interact with it.
    - Discount subclasses encapsulate the discount logic, ensuring that the order and pricing logic remain separate.

- **Abstraction & Polymorphism**:  
    - `Discount` is an abstract base class.  
    - `PercentageDiscount` and `ThresholdDiscount` implement `apply()` so that `Order` can use any discount type without knowing the specifics.

- **Immutable Value Object**:  
    - `Item` is an immutable data object, ensuring no side effects during the operations on the items.

- **Responsibility Separation**:  
    - Each class has a single, clear responsibility:  
        - `Item` → Represents a product.  
        - `Items` → Manages a collection of items.  
        - `Discount` → Calculates discounts.  
        - `Order` → Orchestrates the collection of items and applies discounts.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Item` holds data, `Items` manages the collection, `Discount` applies pricing rules, and `Order` orchestrates the total calculation.
  
- **OCP (Open/Closed Principle)**:  
    - Adding new discounts doesn’t require modifying existing code. Simply subclass the `Discount` class.
  
- **LSP (Liskov Substitution Principle)**:  
    - Any `Discount` subclass can replace another without breaking functionality in `Order`.
  
- **ISP (Interface Segregation Principle)**:  
    - `Discount` has a minimal interface (`apply(total)`). Clients don’t have to implement or use unnecessary methods.

- **DIP (Dependency Inversion Principle)**:  
    - `Order` depends on the `Discount` abstraction, not on concrete discount classes. This keeps the logic decoupled and more maintainable.

---
