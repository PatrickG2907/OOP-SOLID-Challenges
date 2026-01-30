# Intermediate Level OOP & SOLID Challenges

Welcome to the **Intermediate Level** challenges focused on Object-Oriented Programming (OOP) and SOLID principles. These challenges are designed to help you further develop your skills and apply more advanced concepts such as **Strategy Pattern**, **Dependency Injection**, and **Liskov Substitution Principle**.

As you work through these challenges, you'll refine your understanding of key OOP concepts and SOLID principles while tackling real-world problems with multiple design solutions.

---

## Challenge 1: Payment Processing System

### Task
Create a system that supports multiple payment methods:
- **Credit Card**
- **PayPal**
- **Crypto**

### Objectives
- **Open/Closed Principle**: The system should be open for extension (new payment methods) but closed for modification.
- **Dependency Inversion**: High-level modules should not depend on low-level modules; both should depend on abstractions.
- **Strategy Pattern**: Use a strategy pattern to define a family of payment methods and switch between them dynamically.

---

## Challenge 2: Employee Salary Calculator

### Task
Create different types of employees, each calculating their salary differently:
- **Full-time Employee**
- **Part-time Employee**
- **Contractor**

### Objectives
- **Liskov Substitution Principle (LSP)**: Ensure that objects of the base class (e.g., `Employee`) can be replaced with objects of derived classes (e.g., `FullTimeEmployee`, `PartTimeEmployee`, `Contractor`) without affecting the functionality.
- **Polymorphism**: Use polymorphism to define different ways of calculating salaries.
- **Avoid type-checking**: Do not use type-checking or explicit conditionals to handle different employee types.

---

## Challenge 3: Logging Framework

### Task
Design a logging framework that can log messages to:
- **Console**
- **File**
- **Remote Server**

### Objectives
- **Interface Segregation Principle (ISP)**: Split the logging interface into smaller, more specific interfaces (e.g., `ILogToConsole`, `ILogToFile`).
- **Dependency Injection**: Inject dependencies (like the loggers) into the classes that require logging, rather than hard-coding them.

---

## Challenge 4: Smart Home Devices

### Task
Design a system for smart home devices, where devices can include:
- **Light**
- **Fan**

Each device can:
- **Turn on/off**
- Some devices may have additional functionalities (e.g., **Fan** can set speed).

### Objectives
- **Interface Segregation Principle (ISP)**: Devices should implement only the methods they need (e.g., a `Fan` might not need an `On/Off` method if it’s already a part of the general `Device` interface).
- **Avoid forcing unused methods**: Do not force devices to implement methods they don’t use (e.g., a `Light` device shouldn’t have a `SetSpeed` method).

---

## Challenge 5: Online Order System

### Task
Create a system to manage online orders with:
- Items in the order
- Total price calculation
- Discount application

### Objectives
- **Single Responsibility Principle (SRP)**: Keep order storage separate from pricing logic. Pricing should be handled by a separate class, while the order stores the items.
- **Open/Closed Principle**: The discount rules should be easily extendable without modifying existing code (i.e., new discount strategies can be added).

---

## How to Use This Repository

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/OOP-SOLID-Challenges.git
