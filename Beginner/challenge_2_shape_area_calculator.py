from abc import ABC, abstractmethod
import math

class Shape(ABC):
    """OOP: Abstraction – defines the 'Shape' interface
       SOLID (SRP/OCP/LSP/DIP) – clients depend on abstraction, not concrete classes
    """
    @abstractmethod
    def area(self) -> float:
        """Calculate area of the shape"""
        pass


class Rectangle(Shape):
    """OOP: Encapsulation – height and width are private
       SRP – class only represents a rectangle and its rules
       OCP – can extend functionality without modifying existing code
    """
    def __init__(self, height: float, width: float):
        if height <= 0 or width <= 0:  # OOP: enforce class invariants
            raise ValueError("Rectangle dimensions must be positive")
        self._height = height  # Encapsulation: hidden internal state
        self._width = width

    def area(self) -> float:
        """OOP: Polymorphism – implements Shape.area"""
        return self._height * self._width


class Circle(Shape):
    """OOP: Encapsulation – radius is private
       SRP – class only represents a circle and its rules
       OCP – can extend functionality without modifying existing code
    """
    def __init__(self, radius: float):
        if radius <= 0:  # OOP: enforce class invariant
            raise ValueError("Radius must be positive")
        self._radius = radius  # Encapsulation

    def area(self) -> float:
        """OOP: Polymorphism – implements Shape.area"""
        return math.pi * self._radius ** 2


# Example usage (demonstrates polymorphism):
shapes: list[Shape] = [Rectangle(3, 4), Circle(5)]
for shape in shapes:
    print(shape.area())  # Client code depends on abstraction (DIP)
