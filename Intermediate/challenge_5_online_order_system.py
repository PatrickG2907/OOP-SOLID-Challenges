from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Iterable, Optional

# -------------------------------
# Domain (OOP: encapsulation + immutability)
# -------------------------------
@dataclass(frozen=True)  # OOP: immutable value object (SRP)
class Item:
    name: str
    price: float
    quantity: int

class Items:
    """OOP: Encapsulation of a collection of items (SRP)"""
    def __init__(self, items: Optional[Iterable[Item]] = None):
        self._items = list(items) if items else []

    def add_item(self, item: Item):  # OOP: behavior on data
        self._items.append(item)

    def remove_item(self, item: Item):  # OOP: behavior on data
        if item in self._items:
            self._items.remove(item)

    def __iter__(self):  # OOP: iterator support
        return iter(self._items)

    def total(self) -> float:  # SRP: knows how to compute total price
        return sum(item.price * item.quantity for item in self._items)

# -------------------------------
# Discounts (OCP + Liskov Substitution + DIP)
# -------------------------------
class Discount(ABC):  # OOP: abstraction / polymorphism
    """Abstract base class for discounts"""
    @abstractmethod
    def apply(self, total: float) -> float:  # SRP: only transforms total
        pass

class PercentageDiscount(Discount):  # OCP: can add new discount types without modifying Order
    def __init__(self, rate: float):
        if not 0 <= rate <= 1:
            raise ValueError("Discount rate must be between 0 and 1")
        self._rate = rate

    def apply(self, total: float) -> float:  # LSP: can substitute Discount
        return total * (1 - self._rate)

class ThresholdDiscount(Discount):  # OCP + SRP
    """Applies discount only if total exceeds threshold"""
    def __init__(self, threshold: float, rate: float):
        if threshold < 0 or not 0 <= rate <= 1:
            raise ValueError("Invalid threshold or rate")
        self._threshold = threshold
        self._rate = rate

    def apply(self, total: float) -> float:
        if total >= self._threshold:
            return total * (1 - self._rate)
        return total

# -------------------------------
# Order (SRP + DIP)
# -------------------------------
class Order:
    """OOP: orchestrates items and discount (SRP)
       DIP: depends on Discount abstraction, not concrete class"""
    def __init__(self, items: Items, discount: Optional[Discount] = None):
        self._items = items
        self._discount = discount

    def total(self) -> float:
        total = self._items.total()  # SRP: Items handles its own total
        if self._discount:           # DIP: uses discount abstraction
            total = self._discount.apply(total)  # Polymorphism: applies any discount subclass
        return total

# -------------------------------
# Example usage
# -------------------------------
if __name__ == "__main__":
    cart = Items([
        Item("Book", 10, 2),
        Item("Pen", 2, 5)
    ])

    discount = PercentageDiscount(0.1)  # 10% off
    order = Order(cart, discount)

    print(order.total())  # OOP + SOLID in action: total price calculated cleanly
