# Beginner Level OOP & SOLID Challenges

Welcome to the **Beginner Level** challenges focused on Object-Oriented Programming (OOP) and SOLID principles. These challenges will help you build a strong foundation in key OOP concepts like **Encapsulation**, **Inheritance**, **Polymorphism**, and the SOLID principles.

Each challenge is designed to help you apply the theory you’ve learned to practical problems, and to solidify your understanding of OOP design principles.

---

## Challenge 1: User Profile Manager

### Task
Create a system to manage user profiles with the following features:
- Store user information: **name**, **email**, and **age**.
- Update user details.
- Display user information.

### Objectives
- **Single Responsibility Principle (SRP)**: Separate data management from the presentation logic.
- **Encapsulation**: Use private attributes to hide sensitive information and provide controlled access.

---

## Challenge 2: Shape Area Calculator

### Task
Implement the following shapes, each of which should be able to calculate its area:
- **Rectangle**
- **Circle**

### Objectives
- **Inheritance**: Implement a base class `Shape` and extend it to different shapes.
- **Polymorphism**: Override area calculation method in each shape class.
- **Avoid `if/else`**: Do not use `if/else` to determine the type of shape.

---

## Challenge 3: Simple Bank Account

### Task
Create a simple bank account with the following capabilities:
- **Deposit money**
- **Withdraw money**
- **Show account balance**

### Objectives
- **Encapsulation**: Keep the internal balance and transaction logic private.
- **Validation**: Ensure all transactions are valid (e.g., no negative deposits).
- **Avoid exposing internal state**: Do not expose the balance directly; provide methods for users to interact with it.

---

## Challenge 4: Notification Sender

### Task
Create a notification system that can send notifications via:
- **Email**
- **SMS**

### Objectives
- **Abstraction via interfaces (Abstract Base Class - ABC)**: Define a common interface for notification types.
- **Open/Closed Principle**: The system should be open for extension (e.g., adding new notification types) but closed for modification.

---

## Challenge 5: Book Library

### Task
Create a simple library system that:
- Stores a collection of books.
- Allows searching for books by title.
- Displays available books.

### Objectives
- **Single Responsibility Principle (SRP)**: Keep the storage logic separate from the user interface (UI) logic.
- **Separation of Concerns**: Ensure the code related to books (e.g., adding, searching) is distinct from display logic.

---

## How to Use This Repository

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/OOP-SOLID-Challenges.git
