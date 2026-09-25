from typing import Protocol

# =========================
# INTERFACES (ISP)
# =========================

class Switchable(Protocol):
    """OOP: Interface (capability-based)
       SOLID: Interface Segregation Principle (ISP)
    """
    def turn_on(self) -> None: ...
    def turn_off(self) -> None: ...
    @property
    def is_on(self) -> bool: ...

class Adjustable(Protocol):
    """OOP: Interface (capability-based)
       SOLID: ISP — only devices that need adjustment implement this
    """
    def set_value(self, value: int) -> None: ...
    @property
    def value(self) -> int: ...

# =========================
# BEHAVIORS (SRP + COMPOSITION)
# =========================

class OnOffBehavior:
    """OOP: Encapsulated state + behavior
       SOLID: Single Responsibility Principle (SRP)
    """
    def __init__(self, initial_state: bool = False):
        self._state = initial_state

    def turn_on(self) -> None:
        self._state = True

    def turn_off(self) -> None:
        self._state = False

    @property
    def is_on(self) -> bool:
        return self._state

class ValueBehavior:
    """OOP: Encapsulation
       SOLID: SRP — manages numeric state & validation only
    """
    def __init__(self, value: int, min_value: int, max_value: int):
        if min_value > max_value:
            raise ValueError("Invalid value range")
        if not min_value <= value <= max_value:
            raise ValueError("Initial value out of range")

        self._value = value
        self._min = min_value
        self._max = max_value

    def set_value(self, value: int) -> None:
        if not self._min <= value <= self._max:
            raise ValueError("Value out of range")
        self._value = value

    @property
    def value(self) -> int:
        return self._value

# =========================
# DEVICES (COMPOSITION + DIP)
# =========================

class Light:
    """OOP: Concrete class
       SOLID:
       - Uses composition instead of inheritance
       - Depends on abstractions (DIP)
       - Open for extension (add behaviors), closed for modification (OCP)
    """
    def __init__(self, power: OnOffBehavior):
        self._power = power  # Composition

    def turn_on(self) -> None:
        self._power.turn_on()

    def turn_off(self) -> None:
        self._power.turn_off()

    @property
    def is_on(self) -> bool:
        return self._power.is_on

class Fan:
    """OOP: Concrete class
       SOLID:
       - ISP: Implements only needed capabilities
       - DIP: Behaviors injected
       - LSP: Can be used anywhere Switchable/Adjustable is expected
    """
    def __init__(self, power: OnOffBehavior, speed: ValueBehavior):
        self._power = power        # Composition
        self._speed = speed

    def turn_on(self) -> None:
        self._power.turn_on()

    def turn_off(self) -> None:
        self._power.turn_off()

    def set_value(self, value: int) -> None:
        if not self._power.is_on:
            raise RuntimeError("Fan must be on to change speed")
        self._speed.set_value(value)

    @property
    def is_on(self) -> bool:
        return self._power.is_on

    @property
    def value(self) -> int:
        return self._speed.value

# =========================
# USAGE (POLYMORPHISM)
# =========================

def toggle(device: Switchable) -> None:
    """OOP: Polymorphism
       SOLID: LSP — works with ANY Switchable
    """
    if device.is_on:
        device.turn_off()
    else:
        device.turn_on()


fan = Fan(
    power=OnOffBehavior(),
    speed=ValueBehavior(value=1, min_value=1, max_value=5)
)

light = Light(OnOffBehavior())

toggle(fan)
fan.set_value(3)

toggle(light)
