from abc import ABC, abstractmethod
from typing import List

# ==========================
# OOP + SOLID: Interface Segregation (I) & Open/Closed (O)
# ==========================
class Event(ABC):
    """Base Event class (OOP: inheritance, SOLID: Open/Closed)"""
    @abstractmethod
    def name(self) -> str:
        pass

class UserRegistered(Event):
    def __init__(self, username: str):
        if not username or not isinstance(username, str):
            raise ValueError("username must be a non-empty string")
        self.username = username

    def name(self) -> str:
        return "UserRegistered"

class OrderPlaced(Event):
    def __init__(self, order_id: int):
        if not isinstance(order_id, int) or order_id <= 0:
            raise ValueError("order_id must be a positive integer")
        self.order_id = order_id

    def name(self) -> str:
        return "OrderPlaced"

class PaymentFailed(Event):
    def __init__(self, order_id: int, reason: str):
        if not isinstance(order_id, int) or order_id <= 0:
            raise ValueError("order_id must be a positive integer")
        if not reason or not isinstance(reason, str):
            raise ValueError("reason must be a non-empty string")
        self.order_id = order_id
        self.reason = reason

    def name(self) -> str:
        return "PaymentFailed"

# ==========================
# OOP + SOLID: Interface Segregation (I)
# ==========================
class EventListener(ABC):
    """Interface for all event listeners (I)"""
    @abstractmethod
    def handle(self, event: Event):
        pass

# ==========================
# Loose coupling: concrete listeners implement interface (D: Dependency Inversion)
# ==========================
class EmailNotifier(EventListener):
    def handle(self, event: Event):
        if isinstance(event, UserRegistered):
            print(f"Sending welcome email to {event.username}")
        elif isinstance(event, PaymentFailed):
            print(f"Sending payment failure email for order {event.order_id}")

class AnalyticsTracker(EventListener):
    def handle(self, event: Event):
        print(f"Tracking event: {event.name()}")

class FraudDetector(EventListener):
    def handle(self, event: Event):
        if isinstance(event, PaymentFailed):
            print(f"Checking fraud for failed payment on order {event.order_id}")

# ==========================
# Observer Pattern
# ==========================
class EventManager:
    """Manages event registration and notification (OOP: composition, loose coupling)"""
    def __init__(self):
        self.listeners: List[EventListener] = []

    def register_listener(self, listener: EventListener):
        self.listeners.append(listener)

    def notify(self, event: Event):
        for listener in self.listeners:
            listener.handle(event)

# ==========================
# Example Usage
# ==========================
if __name__ == "__main__":
    manager = EventManager()

    # Register listeners (D: Dependency Inversion, loose coupling)
    manager.register_listener(EmailNotifier())
    manager.register_listener(AnalyticsTracker())
    manager.register_listener(FraudDetector())

    # Fire events
    manager.notify(UserRegistered("alice"))
    manager.notify(OrderPlaced(1234))
    manager.notify(PaymentFailed(1234, "Insufficient funds"))

    # This would raise validation errors
    # manager.notify(UserRegistered(""))  # invalid
    # manager.notify(PaymentFailed(0, ""))  # invalid
