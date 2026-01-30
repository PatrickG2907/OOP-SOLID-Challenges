from abc import ABC, abstractmethod
from dataclasses import dataclass

# =========================
# DOMAIN
# =========================
class InvalidPaymentError(Exception):
    """
    SRP:
    - This exception has a single responsibility:
      representing invalid payment input.
    """
    pass

@dataclass(frozen=True)
class PaymentResult:
    """
    SRP:
    - Represents only the outcome of a payment.
    - No logic, no behavior.
    OOP:
    - Encapsulates payment result data into a domain object
      instead of using primitive strings.
    """
    success: bool
    message: str

# =========================
# ABSTRACTION (STRATEGY)
# =========================
class PaymentMethod(ABC):
    """
    OOP:
    - Abstract Base Class defines a contract.
    - Enables polymorphism.
    ISP:
    - Interface is small and focused (one method).
    LSP:
    - Any subclass must be safely substitutable
      for PaymentMethod.
    """
    @abstractmethod
    def pay(self, amount: float) -> PaymentResult:
        pass

class CreditCard(PaymentMethod):
    """
    SRP:
    - Handles ONLY credit card payment behavior.
    OCP:
    - New payment methods can be added without
      modifying existing code.
    """
    def pay(self, amount: float) -> PaymentResult:
        return PaymentResult(
            success=True,
            message=f"{amount} € paid using Credit Card"
        )

class PayPal(PaymentMethod):
    """
    SRP + OCP:
    - Same role as CreditCard, different behavior.
    """

    def pay(self, amount: float) -> PaymentResult:
        return PaymentResult(
            success=True,
            message=f"{amount} € paid using PayPal"
        )


class Crypto(PaymentMethod):
    """
    LSP:
    - Fully interchangeable with other PaymentMethod
      implementations.
    """
    def pay(self, amount: float) -> PaymentResult:
        return PaymentResult(
            success=True,
            message=f"{amount} € paid using Crypto"
        )

# =========================
# HIGH-LEVEL POLICY
# =========================
class PaymentProcessor:
    """
    SRP:
    - Orchestrates the payment flow.
    - Does NOT know how payments are executed.
    DIP:
    - Depends on the abstraction (PaymentMethod),
      not concrete implementations.
    """
    def __init__(self, payment_method: PaymentMethod):
        """
        DIP + OOP:
        - Dependency Injection via constructor.
        - Enables runtime polymorphism.
        """
        self._payment_method = payment_method

    def process(self, amount: float) -> PaymentResult:
        """
        OOP:
        - Delegates behavior to injected strategy.
        SOLID:
        - Validation logic centralized.
        """
        self._validate(amount)
        return self._payment_method.pay(amount)

    def _validate(self, amount: float) -> None:
        """
        SRP:
        - Validation logic separated from payment logic.
        """
        if amount <= 0:
            raise InvalidPaymentError("Amount must be positive")
            
# =========================
# SAMPLE USAGE
# =========================
if __name__ == "__main__":
    """
    OOP:
    - Client code works with abstractions.
    - No conditionals, no type checks.
    """
    methods = [CreditCard(), PayPal(), Crypto()]

    for method in methods:
        processor = PaymentProcessor(method)   # DIP
        result = processor.process(100)         # Polymorphism
        print(result.message)
    try:
        processor = PaymentProcessor(CreditCard())
        processor.process(-50)
    except InvalidPaymentError as e:
        print(f"Error: {e}")
