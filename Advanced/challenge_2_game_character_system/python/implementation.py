from abc import ABC, abstractmethod

# -----------------------
# Attack Strategy (OOP + SOLID)
# -----------------------
class Attack(ABC):
    @abstractmethod
    def attack(self, character) -> int:
        """Perform attack using the character's stats."""
        pass

class Sword(Attack):
    def attack(self, character) -> int:
        return character.power if character.distance_to_target <= character.attack_range else 0

class Bow(Attack):
    def attack(self, character) -> int:
        return character.power if character.distance_to_target <= character.attack_range else 0

class Spell(Attack):
    def attack(self, character) -> int:
        return character.power  # Spells ignore range

# -----------------------
# Ability Strategy (OOP + SOLID)
# -----------------------
class Ability(ABC):
    @abstractmethod
    def use(self, character):
        """Modify the character's state."""
        pass

class Recover(Ability):
    def __init__(self, heal_amount: int):
        self.heal_amount = heal_amount

    def use(self, character):
        character.health += self.heal_amount
        print(f"{character.name} recovers {self.heal_amount} health!")

class IncreaseRange(Ability):
    def __init__(self, increase_amount: int):
        self.increase_amount = increase_amount

    def use(self, character):
        character.attack_range += self.increase_amount
        print(f"{character.name}'s attack range increased by {self.increase_amount}!")

# -----------------------
# Character Base Class (OOP + SOLID)
# -----------------------
class Character:
    def __init__(self, name: str, health: int, power: int, attack_range: int, attack: Attack, ability: Ability):
        if not name:
            raise ValueError("Name cannot be empty")
        if health <= 0 or power <= 0 or attack_range <= 0:
            raise ValueError("Health, power, and range must be positive")

        self.name = name
        self.health = health
        self.power = power
        self.attack_range = attack_range

        self._attack = attack      # ✅ Composition / Strategy
        self._ability = ability    # ✅ Composition / Strategy

        self.distance_to_target = 1  # Default target distance

    def perform_attack(self) -> int:
        damage = self._attack.attack(self)
        print(f"{self.name} attacks for {damage} damage!")
        return damage

    def use_ability(self):
        self._ability.use(self)

# -----------------------
# Specific Characters (OOP + SOLID)
# -----------------------
class Warrior(Character):
    def __init__(self, name: str):
        super().__init__(name, health=100, power=10, attack_range=1, attack=Sword(), ability=Recover(20))

class Archer(Character):
    def __init__(self, name: str):
        super().__init__(name, health=80, power=8, attack_range=5, attack=Bow(), ability=IncreaseRange(2))

class Mage(Character):
    def __init__(self, name: str):
        super().__init__(name, health=60, power=15, attack_range=3, attack=Spell(), ability=Recover(10))

# -----------------------
# Example Usage
# -----------------------
warrior = Warrior("Conan")
archer = Archer("Legolas")
mage = Mage("Gandalf")

warrior.distance_to_target = 1
archer.distance_to_target = 4
mage.distance_to_target = 2

warrior.perform_attack()
archer.perform_attack()
mage.perform_attack()

warrior.use_ability()
archer.use_ability()
mage.use_ability()

print(f"{warrior.name} health: {warrior.health}")
print(f"{archer.name} attack range: {archer.attack_range}")
print(f"{mage.name} health: {mage.health}")
