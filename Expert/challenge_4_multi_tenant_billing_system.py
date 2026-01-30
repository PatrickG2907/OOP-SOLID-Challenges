from abc import ABC, abstractmethod
from dataclasses import dataclass
from decimal import Decimal
from typing import List

# === Invoice ===
@dataclass
class Invoice:
    # OOP: Using a class to encapsulate invoice data
    # SRP: This class only stores invoice details
    customer_name: str
    usage: float
    cost: Decimal
    discount: Decimal
    total: Decimal

# === Pricing Models (Strategy Pattern) ===
class PricingModel(ABC):
    # OOP: Abstract Base Class defines the interface for pricing strategies
    # SOLID: Open/Closed Principle — new pricing models can extend this class without modifying it
    @abstractmethod
    def calculate(self, usage: float) -> Decimal:
        pass

class FlatRatePricing(PricingModel):
    # OOP: Concrete implementation of a strategy
    def __init__(self, rate: float):
        self.rate = Decimal(rate)

    def calculate(self, usage: float) -> Decimal:
        return self.rate

class UsageBasedPricing(PricingModel):
    # OOP: Another concrete strategy
    def __init__(self, price_per_unit: float):
        self.price_per_unit = Decimal(price_per_unit)

    def calculate(self, usage: float) -> Decimal:
        return Decimal(usage) * self.price_per_unit

# === Customers ===
class Customer(ABC):
    # OOP: Abstract Base Class for customers
    # SRP: Only responsible for customer-specific data and discount logic
    def __init__(self, name: str):
        self.name = name

    @abstractmethod
    def get_discount(self) -> Decimal:
        pass

class RegularCustomer(Customer):
    # Liskov Substitution Principle: Can replace Customer anywhere
    def get_discount(self) -> Decimal:
        return Decimal("0.0")

class PremiumCustomer(Customer):
    # Liskov Substitution Principle: Subclass behaves consistently with base class
    DISCOUNT = Decimal("0.1")  # 10% discount

    def get_discount(self) -> Decimal:
        return self.DISCOUNT

# === Factories ===
class CustomerFactory:
    # OOP: Encapsulates object creation
    # SOLID: Open/Closed Principle — easy to add new customer types
    @staticmethod
    def create_customer(customer_type: str, name: str) -> Customer:
        mapping = {
            "regular": RegularCustomer,
            "premium": PremiumCustomer,
        }
        if customer_type.lower() not in mapping:
            raise ValueError(f"Unknown customer type: {customer_type}")
        return mapping[customer_type.lower()](name)

class PricingFactory:
    # OOP: Encapsulates pricing model creation
    # SOLID: Open/Closed Principle — easy to add new pricing strategies
    @staticmethod
    def create_pricing(pricing_type: str, **kwargs) -> PricingModel:
        mapping = {
            "flat": FlatRatePricing,
            "usage": UsageBasedPricing,
        }
        if pricing_type.lower() not in mapping:
            raise ValueError(f"Unknown pricing type: {pricing_type}")
        return mapping[pricing_type.lower()](**kwargs)

# === Billing Service ===
class BillingService:
    # SRP: Only responsible for generating invoices
    # OOP: Uses composition to work with Customer and PricingModel abstractions
    # DIP: Depends on abstractions, not concrete classes
    def __init__(self):
        self.invoices: List[Invoice] = []

    def generate_invoice(
        self, customer: Customer, pricing_model: PricingModel, usage: float
    ) -> Invoice:
        # Validation - basic error handling
        if usage < 0:
            raise ValueError("Usage cannot be negative")
        cost = pricing_model.calculate(usage)       # Strategy pattern applied
        discount = cost * customer.get_discount()   # Polymorphism: customer type decides discount
        total = cost - discount
        invoice = Invoice(customer.name, usage, cost, discount, total)
        self.invoices.append(invoice)
        return invoice

# === Example Usage ===
if __name__ == "__main__":
    # Factory Pattern: Create customers and pricing dynamically
    alice = CustomerFactory.create_customer("premium", "Alice")
    bob = CustomerFactory.create_customer("regular", "Bob")

    usage_pricing = PricingFactory.create_pricing("usage", price_per_unit=5)
    flat_pricing = PricingFactory.create_pricing("flat", rate=200)

    billing_service = BillingService()

    # Generate invoices
    invoice1 = billing_service.generate_invoice(alice, usage_pricing, usage=100)
    invoice2 = billing_service.generate_invoice(bob, flat_pricing, usage=50)

    # Print invoices
    for invoice in billing_service.invoices:
        print(
            f"{invoice.customer_name}: Usage={invoice.usage}, "
            f"Cost=${invoice.cost}, Discount=${invoice.discount}, Total=${invoice.total}"
        )
