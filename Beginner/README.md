# OOP & SOLID Challenges (Beginner Level)

## Challenge 1: User Profile Manager

### Task
Create a system to manage user profiles:
- Store name, email, and age.
- Update user details.
- Display user information.

### Objectives
- **SRP (Single Responsibility Principle)**
- **Encapsulation**
  
### Explanations

**OOP Principles**
- **Abstraction**: 
    - *"Expose what an object does, hide how it does it."*  
    - The `User` class represents a real-world user, and clients interact with `name`, `email`, `age`, and `get_info()` without knowing how validation or storage works.  
    - **Benefit**: You can change the internal implementation without breaking external code.
  
- **Encapsulation**: 
    - *"Protect an object’s internal state and control how it’s accessed."*  
    - The internal attributes (`_name`, `_email`, `_age`) are private and access is controlled via `@property` setters with validation.  
    - **Benefit**: The `User` object can never be put into an invalid state accidentally.
  
- **Inheritance**: 
    - *"Allow one class to derive behavior from another."*  
    - Not explicitly used, but supported. A subclass (e.g., `AdminUser`) would inherit validation and behavior from `User`.  
    - **Benefit**: Your base class is safe to extend without modification.
  
- **Polymorphism**: 
    - *"Different objects can be used interchangeably via the same interface."*  
    - Any subclass of `User` can be treated as a `User`, and `get_info()` can be overridden while preserving expected behavior.  
    - **Benefit**: Code depending on `User` doesn’t break when new user types appear.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - *"A class should have one reason to change."*  
    - The `User` class is only responsible for holding user data and enforcing user-related rules. It does not handle UI, printing, or networking.  
    - **Benefit**: Changes in UI or storage don’t affect the `User` class.
  
- **OCP (Open/Closed Principle)**:  
    - *"Open for extension, closed for modification."*  
    - Validation rules can be extended (e.g., stricter email checks). External code doesn’t need to change how it sets `user.email`.  
    - **Benefit**: You can evolve behavior without breaking consumers.
  
- **LSP (Liskov Substitution Principle)**:  
    - *"Subtypes must be substitutable for their base type."*  
    - Any subclass of `User` must respect validation rules (e.g., non-negative age, valid name/email).  
    - **Benefit**: A function expecting a `User` won’t break if given a subclass.
  
- **ISP (Interface Segregation Principle)**:  
    - *"Clients shouldn’t depend on methods they don’t use."*  
    - `User` exposes a small, focused API: `name`, `email`, `age`, `get_info()`.  
    - **Benefit**: No bloated interface or unused methods.
  
- **DIP (Dependency Inversion Principle)**:  
    - *"Depend on abstractions, not concrete implementations."*  
    - `User` does not depend on printing, databases, or frameworks. Higher-level code depends on the `User` interface, not its internals.  
    - **Benefit**: The same `User` works in CLI, API, GUI, and tests.

---

## Challenge 2: Shape Area Calculator

### Task
Implement shapes like:
- Rectangle
- Circle

Each should calculate its area.

### Objectives
- **Inheritance**
- **Polymorphism**
- **Avoid if/else for shape types**

### Explanations

**OOP Principles**
- **Abstraction**:  
    - `Shape` defines the interface for calculating area without specifying how the area is calculated. Clients use `Shape` type, not concrete classes (`Rectangle`, `Circle`).  

- **Encapsulation**:  
    - The attributes `_height`, `_width`, and `_radius` are private and invariant rules are enforced in the constructors.  

- **Polymorphism**:  
    - Both `Rectangle` and `Circle` implement `area()`, and they can be used interchangeably via the `Shape` interface.  

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - Each shape class has only one reason to change: its shape’s behavior and rules.  

- **OCP (Open/Closed Principle)**:  
    - New shapes (e.g., `Triangle`) can be added without modifying `Rectangle`, `Circle`, or client code.  

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `Shape` can replace a `Shape` in client code safely, as invariants are enforced.  

- **ISP (Interface Segregation Principle)**:  
    - `Shape` exposes only the minimal `area()` method; clients aren’t forced to depend on unused methods.  

- **DIP (Dependency Inversion Principle)**:  
    - Client code depends on `Shape` abstraction, not on concrete classes (`Rectangle` or `Circle`).

---

## Challenge 3: Simple Bank Account

### Task
Create a bank account that can:
- Deposit money
- Withdraw money
- Show balance

### Objectives
- **Encapsulation**
- **Validation Logic**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - `_balance` is private and exposed as a read-only property. State can only be changed through `deposit` and `withdraw` methods.  

- **Abstraction**:  
    - Users interact with what the account does (`deposit`, `withdraw`), not how it stores or validates data. Internal validation and rules are hidden.  

- **Data Integrity / Invariants**:  
    - No negative balances or invalid transaction amounts; invalid states cannot be created.  

- **Behavior-centric Design**:  
    - Logic (e.g., money rules) lives with the data (e.g., account), making the design intuitive and cohesive.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - `BankAccount` is responsible only for managing account state and rules.  

- **OCP (Open/Closed Principle)**:  
    - The class can be extended (e.g., interest accounts, overdraft rules) without modifying existing code. New behavior can be added via subclasses or composition.  

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `BankAccount` can replace it without breaking client code, as invariants are preserved.  

- **ISP (Interface Segregation Principle)**:  
    - The public API is small and focused: `deposit`, `withdraw`, `balance`.  

- **DIP (Dependency Inversion Principle)**:  
    - Not applied prematurely, but allows for future introduction of `Account` abstraction if required.

---

## Challenge 4: Notification Sender

### Task
Send notifications via:
- Email
- SMS

### Objectives
- **Abstraction via Interfaces**
- **Open/Closed Principle**

### Explanations

**OOP Principles**
- **Encapsulation**:  
    - `_send` and `_validate` are internal methods that hide implementation details. `send()` is the public method clients call.  

- **Abstraction**:  
    - `Notification` defines what a notification can do (`send`), but not how it’s delivered. Clients interact with the abstract `Notification` interface, not concrete `Email` or `SMS` classes.  

- **Polymorphism**:  
    - `Email`, `SMS`, or any new notification type can be used interchangeably via the `Notification` interface.  

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - The base class handles validation and orchestration; subclasses handle only the specific delivery mechanism (`Email`, `SMS`).  

- **OCP (Open/Closed Principle)**:  
    - New notification types can be added without modifying existing code; only `_send()` needs to be implemented for a new channel.  

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `Notification` can replace it without breaking client code. The `send()` workflow remains consistent across all subclasses.  

- **ISP (Interface Segregation Principle)**:  
    - The public API is minimal (`send()`), exposing only what clients need. Clients don’t depend on unnecessary methods like `_send()` or `_validate()`.  

- **DIP (Dependency Inversion Principle)**:  
    - High-level code depends only on the `Notification` abstraction, not concrete `Email` or `SMS` classes. This allows flexible substitution of notification types.

---

## Challenge 5: Book Library

### Task
Create a library that:
- Stores books
- Allows searching by title
- Displays available books

### Objectives
- **SRP (Single Responsibility Principle)**
- **Open/Closed Principle**

### Explanations

**OOP Principles**
- **Classes & Objects**:  
    - `Book`, `BookRepository`, `Library`, `SearchStrategy` (and its subclasses) are all classes.  
    - Individual books are objects of the `Book` class.  

- **Encapsulation**:  
    - `Book`’s `title` and `author` are private with read-only access (`@property`).  
    - `BookRepository`’s `_books` list is private; external access is only via `add_book()` and `list_books()`.  

- **Abstraction**:  
    - `BookRepositoryInterface` defines what a repository should do, not how it stores books.  
    - `SearchStrategy` defines how a search should behave, without specifying the concrete implementation.  

- **Inheritance & Polymorphism**:  
    - `TitleSearchStrategy` and `AuthorSearchStrategy` inherit from `SearchStrategy`.  
    - Any `SearchStrategy` object can be passed to `Library.search_books()` (polymorphism).  

- **Separation of Concerns**:  
    - `Library` handles data management, not printing. The application layer handles presentation.

**SOLID Principles**
- **SRP (Single Responsibility Principle)**:  
    - Each class has a single responsibility.  

- **OCP (Open/Closed Principle)**:  
    - `SearchStrategy` can be extended without modifying existing classes.  

- **LSP (Liskov Substitution Principle)**:  
    - Any subclass of `SearchStrategy` can replace another without breaking `Library`.  

- **ISP (Interface Segregation Principle)**:  
    - `BookRepositoryInterface` provides only the methods that clients need.  

- **DIP (Dependency Inversion Principle)**:  
    - `Library` depends on the abstraction `BookRepositoryInterface`, not the concrete `BookRepository`.

---
