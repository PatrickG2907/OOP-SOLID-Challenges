# OOP & SOLID Challenges (Advanced Level)

## Challenge 1: Report Generation System

### Task
Generate reports in:
- PDF
- CSV

### Objectives
- **Strategy Pattern**
- **Dependency Inversion**
- **Open/Closed Principle**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - Report name is private (`_name`) and accessed via a property (`name`).
    - Each class handles its own responsibilities internally, ensuring a clear separation of concerns.
  
- **Inheritance**:  
    - `PdfReport` and `CsvReport` inherit from `ReportStrategy`, sharing common behavior (e.g., name validation) and interface (`generate_report()`).

- **Polymorphism**:  
    - `ReportManager` treats all reports as `ReportStrategy` objects and can call `generate_report()` on any report type without caring about the concrete implementation.
  
- **Abstraction**:  
    - `ReportStrategy` defines an abstract interface for all reports.
    - Concrete reports implement the actual generation logic.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `PdfReport` and `CsvReport` are responsible only for generating specific report content.  
    - `ReportManager` is responsible for managing collections of reports.

- **OCP (Open/Closed Principle)**:  
    - The system is open to extension (new report types) without modifying existing code. For instance, adding `ExcelReport` or `JsonReport` requires just a new class.
  
- **LSP (Liskov Substitution Principle)**:  
    - Any `ReportStrategy` subclass can replace another in `ReportManager` without breaking behavior.

- **ISP (Interface Segregation Principle)**:  
    - `ReportStrategy` provides a simple interface (`generate_report()`), without forcing irrelevant methods on subclasses.

- **DIP (Dependency Inversion Principle)**:  
    - `ReportManager` depends on the abstraction (`ReportStrategy`), not on concrete report classes. This decouples the manager from specific report types.

---

## Challenge 2: Game Character System

### Task
Characters:
- Warrior
- Mage
- Archer

They have different attack behaviors and abilities.

### Objectives
- **Composition Over Inheritance**
- **Strategy Pattern**
- **Liskov Substitution Principle**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - `Attack` and `Ability` are abstract base classes (ABC) defining a common interface (`attack()` / `use()`).
    - Characters interact with `Attack` and `Ability` objects without caring about their concrete implementation.

- **Encapsulation**:  
    - The character’s internal behavior (the `_attack` and `_ability` objects) is hidden.  
    - Methods like `perform_attack()` and `use_ability()` provide a controlled interface for interacting with these behaviors.

- **Inheritance**:  
    - `Warrior`, `Archer`, `Mage` inherit from `Character` and share common properties (e.g., `name`, `health`, `power`).  
    - They override only what needs to be specialized (e.g., initial stats and attack/ability composition).

- **Polymorphism**:  
    - Any `Character` can call `perform_attack()` or `use_ability()` without knowing the exact subclass.  
    - The concrete `Attack` and `Ability` classes define behavior at runtime, enabling flexible swapping of strategies (e.g., `Sword` → `Bow`).

- **Composition / Strategy Pattern**:  
    - `Character` has an `Attack` and `Ability` object instead of hardcoding behavior, allowing swapping or adding new attack/ability behaviors without changing the `Character` class.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `Character`: Manages character state (e.g., `health`, `power`).  
    - `Attack`: Handles attack logic.  
    - `Ability`: Handles ability logic.

- **OCP (Open/Closed Principle)**:  
    - You can add new `Character` subclasses, `Attack` types, or `Ability` types without modifying existing classes.

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `Character` can replace another in code expecting a `Character`.

- **ISP (Interface Segregation Principle)**:  
    - `Attack` and `Ability` have small, focused interfaces (`attack()`, `use()`), so subclasses aren’t forced to implement unnecessary methods.

- **DIP (Dependency Inversion Principle)**:  
    - `Character` depends on the abstractions `Attack` and `Ability` rather than concrete classes like `Sword` or `Recover`. This allows flexible swapping of behaviors at runtime.

---

## Challenge 3: File Storage System

### Task
Store files in:
- Local storage
- Cloud storage

### Objectives
- **Strategy Pattern**
- **Dependency Injection**
- **Open/Closed Principle**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - `Storage` is an abstract base class (interface) defining `save_file()` and `read_file()`.
    - Ensures that code depends on what storage does, not how it does it.

- **Inheritance / Polymorphism**:  
    - `LocalStorage` and `CloudStorage` inherit from `Storage` and implement its methods.
    - `FileManager` can use any subclass interchangeably (polymorphism).

- **Composition / Dependency Injection**:  
    - `FileManager` is composed with a `Storage` object, which can be injected (i.e., it doesn't create `Storage` directly).

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `LocalStorage` and `CloudStorage` manage local/cloud files.  
    - `FilenameValidator` and `DirectoryValidator` validate filenames and directories.  
    - `FileManager` orchestrates file operations.

- **OCP (Open/Closed Principle)**:  
    - You can add new storage types (e.g., `DatabaseStorage`, `EncryptedStorage`) without modifying existing code.

- **LSP (Liskov Substitution Principle)**:  
    - Any `Storage` subclass can replace `Storage` in `FileManager` without breaking behavior.

- **ISP (Interface Segregation Principle)**:  
    - The `Storage` interface only exposes the methods needed for storage operations (`save_file()`, `read_file()`), keeping it lean.

- **DIP (Dependency Inversion Principle)**:  
    - `FileManager` depends on the abstraction (`Storage`), not concrete implementations. Concrete storage classes are injected via the constructor.

---

## Challenge 4: E-Commerce Shipping Calculator

### Task
Shipping types:
- Standard
- Express
- International

Rules vary by destination and weight.

### Objectives
- **Strategy Pattern**
- **Open/Closed Principle**
- **Interface Segregation**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - Data (`ShippingInfo`, `Rule`) and behavior (`apply()` in rules, `calculate()` in shipping types) are kept in separate classes.  
    - `_rule` in `ShippingRule` is private to hide internal details.

- **Abstraction**:  
    - `ShippingRule` and `ShippingType` are abstract base classes.  
    - Users interact with the abstract interface without knowing the specific implementation.

- **Polymorphism**:  
    - Shipping types can use any object implementing the `ShippingRule` interface.  
    - Rules can be swapped or extended without changing shipping types.

- **Inheritance**:  
    - `WeightRule` and `DestinationRule` inherit from `ShippingRule`.  
    - `StandardShipping`, `ExpressShipping`, `InternationalShipping` inherit from `ShippingType`.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `ShippingInfo` and `ShippingRule` store data.  
    - `ShippingRule` applies a rule, and `ShippingType` calculates shipping using rules.

- **OCP (Open/Closed Principle)**:  
    - Adding new rules or shipping types does not require modifying existing classes.

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `ShippingRule` or `ShippingType` can replace the base class without breaking behavior.

- **ISP (Interface Segregation Principle)**:  
    - No class is forced to implement methods it doesn’t use.  
    - Shipping types only need `calculate()`.  
    - Rules only need `apply()`.

- **DIP (Dependency Inversion Principle)**:  
    - High-level shipping types depend on the abstract `ShippingRule` interface, not on concrete implementations like `WeightRule` or `DestinationRule`.

---

## Challenge 5: Authentication System

### Task
Support:
- Password-based login
- OAuth
- API tokens

### Objectives
- **Interface Segregation**
- **Dependency Injection**
- **Secure Responsibility Separation**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - Sensitive data like the password in `User` is private and accessed only via `check_password()`.

- **Abstraction**:  
    - Interfaces like `Authenticator`, `OAuthAuthenticator`, `UserRepository`, and `TokenRepository` hide implementation details, allowing code to depend on what something does, not how it does it.

- **Polymorphism**:  
    - `AuthenticationService` can use any authenticator interchangeably because they all implement the `Authenticator` interface.

- **Inheritance**:  
    - `OAuthAuthenticator` extends `Authenticator`, adding OAuth-specific behavior like `get_redirect_url()`.

- **Composition Over Inheritance**:  
    - Authenticators use repositories or clients via injection, making the system flexible and decoupled.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - Repositories manage data, authenticators handle authentication, and `AuthenticationService` coordinates login.

- **OCP (Open/Closed Principle)**:  
    - You can add new authenticators without changing existing classes.

- **LSP (Liskov Substitution Principle)**:  
    - Any authenticator can replace another in `AuthenticationService` without breaking functionality.

- **ISP (Interface Segregation Principle)**:  
    - OAuth-specific methods are only required for OAuth authenticators; other authenticators don’t implement irrelevant methods.

- **DIP (Dependency Inversion Principle)**:  
    - High-level modules like `AuthenticationService` depend on abstractions (`Authenticator`) rather than concrete classes. Concrete authenticators depend on repositories via their interfaces.

---
