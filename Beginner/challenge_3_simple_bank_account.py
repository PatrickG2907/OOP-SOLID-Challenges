# =========================
# Domain-specific exceptions
# =========================
# OOP: Expresses domain concepts explicitly
# SOLID (SRP): Each exception represents one specific error case

class InvalidAmountError(Exception):
    pass

class InsufficientFundsError(Exception):
    pass

# =========================
# BankAccount domain model
# =========================
# OOP:
# - Encapsulates state (_balance)
# - Exposes behavior (deposit, withdraw)
# - Protects invariants (no invalid state)
#
# SOLID:
# - SRP: Responsible only for managing account balance
# - OCP: Can be extended (interest, overdraft rules) without modification
# - LSP: Any subclass can safely replace BankAccount
# - ISP: Minimal, cohesive public interface
# - DIP: High-level code can later depend on an Account abstraction if needed

class BankAccount:
    def __init__(self, balance: float = 0.0):
        # OOP: Constructor enforces valid initial state
        # SOLID (SRP): Validation logic belongs to the domain model
        if balance < 0:
            raise InvalidAmountError("Initial balance cannot be negative")
        self._balance = balance  # Encapsulation: internal state is protected

    def deposit(self, amount: float) -> None:
        # OOP: Behavior operates on internal state
        # SOLID: Business rules are enforced inside the class
        self._validate_amount(amount)
        self._balance += amount

    def withdraw(self, amount: float) -> None:
        # OOP: State changes only through controlled methods
        self._validate_amount(amount)

        # SOLID (SRP): This class owns overdraft rules
        if amount > self._balance:
            raise InsufficientFundsError("Insufficient funds")

        self._balance -= amount

    @property
    def balance(self) -> float:
        # OOP: Read-only access to internal state (encapsulation)
        return self._balance

    @staticmethod
    def _validate_amount(amount: float) -> None:
        # OOP: Internal helper method
        # SOLID (SRP): Validation logic is centralized and reusable
        if amount <= 0:
            raise InvalidAmountError("Amount must be greater than zero")

# =========================
# Example usage
# =========================

def main():
    # Create a new account with initial balance
    try:
        account = BankAccount(100.0)
        print(f"Initial balance: ${account.balance:.2f}")
    except InvalidAmountError as e:
        print("Error creating account:", e)

    # Deposit some money
    try:
        account.deposit(50.0)
        print(f"Balance after deposit: ${account.balance:.2f}")
    except InvalidAmountError as e:
        print("Deposit error:", e)

    # Withdraw some money
    try:
        account.withdraw(30.0)
        print(f"Balance after withdrawal: ${account.balance:.2f}")
    except (InvalidAmountError, InsufficientFundsError) as e:
        print("Withdrawal error:", e)

    # Attempt to withdraw more than balance
    try:
        account.withdraw(200.0)
    except (InvalidAmountError, InsufficientFundsError) as e:
        print("Withdrawal error:", e)

    # Attempt to deposit an invalid amount
    try:
        account.deposit(-20.0)
    except InvalidAmountError as e:
        print("Deposit error:", e)

if __name__ == "__main__":
    main()
