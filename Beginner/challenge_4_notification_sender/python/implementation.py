from abc import ABC, abstractmethod

# =========================
# Domain-specific exception
# =========================
class InvalidNotificationError(Exception):
    """Raised when a notification message is invalid (empty)."""
    pass

# =========================
# Notification abstraction
# =========================
class Notification(ABC):
    """Abstract base class for notifications."""

    def send(self, message: str) -> None:
        """Public API: validates and sends a notification."""
        self._validate(message)
        self._send(message)

    def _validate(self, message: str) -> None:
        if not message:
            raise InvalidNotificationError("Notification message cannot be empty")

    @abstractmethod
    def _send(self, message: str) -> None:
        """Subclasses implement actual delivery."""
        pass

# =========================
# Concrete implementations
# =========================
class Email(Notification):
    def _send(self, message: str) -> None:
        print(f"Sending EMAIL: {message}")

class SMS(Notification):
    def _send(self, message: str) -> None:
        print(f"Sending SMS: {message}")

# =========================
# Example usage
# =========================
def notify(notification: Notification, message: str) -> None:
    """Client code only depends on the abstraction."""
    notification.send(message)

if __name__ == "__main__":
    email_notifier = Email()
    sms_notifier = SMS()

    notify(email_notifier, "Welcome! Your account is active.")
    notify(sms_notifier, "Your verification code is 123456.")

    # Uncommenting the next line would raise InvalidNotificationError
    # notify(email_notifier, "")
