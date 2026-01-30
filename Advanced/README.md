# Advanced Level OOP & SOLID Challenges

Welcome to the **Advanced Level** challenges focused on Object-Oriented Programming (OOP) and SOLID principles. These challenges will help you tackle real-world, complex systems by applying advanced design patterns and principles like **Strategy Pattern**, **Dependency Inversion**, and **Composition Over Inheritance**.

These challenges are meant for developers who have a solid understanding of OOP and want to deepen their skills by working on systems that require flexible, scalable, and maintainable architectures.

---

## Challenge 1: Report Generation System

### Task
Create a system that can generate reports in the following formats:
- **PDF**
- **CSV**

### Objectives
- **Strategy Pattern or Factory Pattern**: Use the Strategy pattern to define different report generation strategies or a Factory pattern to create report objects.
- **Dependency Inversion**: Depend on abstractions (interfaces) rather than concrete classes for report generation.
- **Open/Closed Principle**: The system should be open for new report formats (like Excel or HTML) without modifying existing code.

---

## Challenge 2: Game Character System

### Task
Create a character system where different types of characters have different attack behaviors and abilities:
- **Warrior**
- **Mage**
- **Archer**

### Objectives
- **Composition Over Inheritance**: Use composition to define common behaviors (e.g., `AttackBehavior`, `Ability`) instead of relying solely on inheritance.
- **Strategy Pattern**: Implement the Strategy pattern to handle the various attack behaviors and abilities for each character.
- **Liskov Substitution Principle**: Ensure that derived classes (e.g., `Mage`, `Warrior`) can be substituted for the base `Character` class without altering the expected behavior.

---

## Challenge 3: File Storage System

### Task
Create a file storage system that can store files in:
- **Local storage**
- **Cloud storage**

### Objectives
- **Dependency Inversion**: The system should depend on abstractions (interfaces like `IStorage`) instead of specific storage implementations (`LocalStorage`, `CloudStorage`).
- **Abstraction of Infrastructure**: Abstract the details of the infrastructure (storage) away from the core business logic to improve flexibility and scalability.
- **Testability**: Ensure the system is easy to test by using dependency injection and mocking the storage infrastructure.

---

## Challenge 4: E-Commerce Shipping Calculator

### Task
Create a shipping calculator that supports different types of shipping:
- **Standard**
- **Express**
- **International**

Shipping rules vary by destination and weight.

### Objectives
- **Open/Closed Principle**: The system should allow for the addition of new shipping types and rules without modifying existing code.
- **Avoid Conditional Explosion**: Avoid using excessive `if/else` statements or switch cases to handle different shipping types.
- **Strategy Pattern**: Use the Strategy pattern to handle different shipping strategies based on destination and weight.

---

## Challenge 5: Authentication System

### Task
Create an authentication system that supports:
- **Password-based login**
- **OAuth**
- **API tokens**

### Objectives
- **Interface Segregation Principle (ISP)**: Create smaller, focused interfaces for different authentication methods (e.g., `PasswordAuth`, `OAuthAuth`).
- **Dependency Inversion**: The authentication system should depend on abstract interfaces rather than concrete classes for different authentication methods.
- **Secure Responsibility Separation**: Ensure clear separation of concerns, keeping authentication logic isolated from other system components.

---

## How to Use This Repository

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/OOP-SOLID-Challenges.git
