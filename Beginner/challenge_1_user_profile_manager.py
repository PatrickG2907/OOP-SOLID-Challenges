class User:  # OOP: Abstraction – models a real-world User concept
    def __init__(self, name: str, email: str, age: int):
        # Encapsulation: assignment goes through setters, not direct field access
        # SOLID (SRP): constructor ensures a valid User is created
        self.name = name
        self.email = email
        self.age = age

    @property
    def name(self) -> str:
        # Encapsulation: internal state (_name) is hidden
        return self._name

    @name.setter
    def name(self, value: str):
        # OOP: Invariant enforcement – User cannot have an empty name
        # SOLID (OCP): validation rules can evolve without changing external code
        if not value:
            raise ValueError("Name cannot be empty")
        self._name = value

    @property
    def email(self) -> str:
        # Encapsulation: controlled read access
        return self._email

    @email.setter
    def email(self, value: str):
        # OOP: Data integrity + business rule enforcement
        # SOLID (SRP): validation lives with the data it protects
        if not value or "@" not in value:
            raise ValueError("Invalid email address")
        self._email = value

    @property
    def age(self) -> int:
        # Encapsulation: prevents uncontrolled modification
        return self._age

    @age.setter
    def age(self, value: int):
        # OOP: Class invariant – age must be non-negative
        # SOLID (LSP): subclasses must respect this rule
        if value < 0:
            raise ValueError("Age cannot be negative")
        self._age = value

    def get_info(self) -> str:
        # OOP: Abstraction – caller doesn't know how info is assembled
        # SOLID (DIP): returns data instead of depending on I/O mechanisms
        return (
            f"Name: {self.name}\n"
            f"Email: {self.email}\n"
            f"Age: {self.age}"
        )

# Creating a new user
user = User(name="Alice", email="alice@example.com", age=30)

# Display user information
print("Initial user info:")
print(user.get_info())

# Update user details using setters
user.name = "Alice Smith"
user.email = "alice.smith@example.com"
user.age = 31

# Display updated information
print("\nUpdated user info:")
print(user.get_info())

# Attempting invalid update (will raise ValueError)
try:
    user.age = -5
except ValueError as e:
    print("\nError:", e)
