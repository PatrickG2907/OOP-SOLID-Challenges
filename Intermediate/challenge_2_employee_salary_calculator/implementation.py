from abc import ABC, abstractmethod

# =========================
# OOP + SOLID: Abstraction
# =========================
class PayPolicy(ABC):
    """
    Abstraction that defines how pay is calculated.
    """
    @abstractmethod
    def calculate_salary(self) -> int:
        pass

# =========================
# OOP: Polymorphism
# SOLID: Open/Closed Principle
# =========================
class FullTimePay(PayPolicy):

    def __init__(self, monthly_salary: int):
        if monthly_salary <= 0:
            raise ValueError("Monthly salary must be positive")
        self._monthly_salary = monthly_salary

    def calculate_salary(self) -> int:
        return self._monthly_salary

class PartTimePay(PayPolicy):

    def __init__(self, hourly_rate: int, hours: int):
        if hourly_rate <= 0 or hours < 0:
            raise ValueError("Invalid hourly data")
        self._hourly_rate = hourly_rate
        self._hours = hours

    def calculate_salary(self) -> int:
        return self._hourly_rate * self._hours

class ContractorPay(PayPolicy):

    def __init__(self, base_pay: int, bonus: int):
        if base_pay <= 0 or bonus < 0:
            raise ValueError("Invalid contractor pay")
        self._base_pay = base_pay
        self._bonus = bonus

    def calculate_salary(self) -> int:
        return self._base_pay + self._bonus

# =========================
# OOP: Composition
# SOLID: Dependency Inversion Principle
# =========================
class Employee:
    """
    Employee does NOT know how salary is calculated.
    It depends on an abstraction (PayPolicy).
    """

    def __init__(self, name: str, pay_policy: PayPolicy):
        self._name = name
        self._pay_policy = pay_policy

    def get_salary(self) -> int:
        return self._pay_policy.calculate_salary()

# =========================
# Client Code
# =========================
employees = [
    Employee("Alice", FullTimePay(4000)),
    Employee("Bob", PartTimePay(30, 80)),
    Employee("Charlie", ContractorPay(2000, 500))
]

total_payroll = sum(e.get_salary() for e in employees)
print(f"Total payroll: {total_payroll}")
