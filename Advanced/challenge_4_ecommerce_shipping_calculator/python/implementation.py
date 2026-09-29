from dataclasses import dataclass
from abc import ABC, abstractmethod
from typing import List, Tuple

# ----------------------------
# Data Classes
# ----------------------------
@dataclass(frozen=True)
class ShippingInfo:
    destination: str
    weight: float

@dataclass(frozen=True)
class Rule:
    limits: List       # Can be numbers for weight or strings for destination
    prices: List[float]
    delivery_times: List[int]  # In days

# ----------------------------
# Abstract Rule Interface
# ----------------------------
class ShippingRule(ABC):
    def __init__(self, rule: Rule):
        self._rule = rule

    @abstractmethod
    def apply(self, info: ShippingInfo) -> Tuple[float, int]:
        """Return (price, delivery_time) if applicable, else raise ValueError"""
        pass

    @abstractmethod
    def rule_error(self) -> str:
        pass

# ----------------------------
# Concrete Rules
# ----------------------------
class WeightRule(ShippingRule):
    def apply(self, info: ShippingInfo) -> Tuple[float, int]:
        for i, limit in enumerate(self._rule.limits):
            if info.weight <= limit:
                return self._rule.prices[i], self._rule.delivery_times[i]
        raise ValueError(self.rule_error())

    def rule_error(self) -> str:
        return "Weight exceeds allowed limits!"

class DestinationRule(ShippingRule):
    def apply(self, info: ShippingInfo) -> Tuple[float, int]:
        for i, limit in enumerate(self._rule.limits):
            if info.destination == limit:
                return self._rule.prices[i], self._rule.delivery_times[i]
        raise ValueError(self.rule_error())

    def rule_error(self) -> str:
        return "Destination not supported!"

# ----------------------------
# Abstract Shipping Type
# ----------------------------
class ShippingType(ABC):
    def __init__(self, info: ShippingInfo, rules: List[ShippingRule]):
        self.info = info
        self.rules = rules

    @abstractmethod
    def calculate(self) -> Tuple[float, int]:
        """Return price and delivery time"""
        pass

# ----------------------------
# Concrete Shipping Types
# ----------------------------
class StandardShipping(ShippingType):
    def calculate(self) -> Tuple[float, int]:
        for rule in self.rules:
            try:
                return rule.apply(self.info)
            except ValueError:
                continue
        raise ValueError("No applicable shipping rule found for Standard shipping!")

class ExpressShipping(ShippingType):
    def calculate(self) -> Tuple[float, int]:
        for rule in self.rules:
            try:
                return rule.apply(self.info)
            except ValueError:
                continue
        raise ValueError("No applicable shipping rule found for Express shipping!")

class InternationalShipping(ShippingType):
    def calculate(self) -> Tuple[float, int]:
        for rule in self.rules:
            try:
                return rule.apply(self.info)
            except ValueError:
                continue
        raise ValueError("No applicable shipping rule found for International shipping!")

# ----------------------------
# Usage Example
# ----------------------------
if __name__ == "__main__":
    # Define rules
    weight_rule = Rule(
        limits=[1, 5, 10],
        prices=[5, 10, 15],
        delivery_times=[5, 4, 3]
    )

    destination_rule = Rule(
        limits=["US", "EU"],
        prices=[7, 12],
        delivery_times=[5, 7]
    )

    # Create Rule objects
    weight_rule_obj = WeightRule(weight_rule)
    destination_rule_obj = DestinationRule(destination_rule)

    # Shipping info
    info = ShippingInfo(destination="US", weight=3)

    # Standard shipping with multiple rules
    standard = StandardShipping(info, rules=[weight_rule_obj, destination_rule_obj])

    price, days = standard.calculate()
    print(f"Standard Shipping: ${price}, {days} days")

    # Express shipping example
    express = ExpressShipping(info, rules=[weight_rule_obj, destination_rule_obj])
    price, days = express.calculate()
    print(f"Express Shipping: ${price}, {days} days")
