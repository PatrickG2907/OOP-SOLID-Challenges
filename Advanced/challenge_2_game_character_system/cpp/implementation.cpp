#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>


// Strategy interface for attack behavior.
// Notes:
//  - Abstract strategy interface for character attacks.
//  - virtual + = 0 makes attack() pure virtual, so each concrete attack
//    strategy must provide its own implementation.
//  - std::string_view is passed by value because it is a lightweight,
//    non-owning read-only view of the character name.
//  - Trailing const means executing the attack does not modify the strategy object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class AttackBehavior {
public:
    virtual ~AttackBehavior() = default;

    virtual void attack(std::string_view character_name) const = 0;
};


// Concrete attack strategy implementing AttackBehavior. It can be substituted 
// anywhere an AttackBehavior is expected, demonstrating runtime polymorphism and 
// Liskov substitution.
// Notes:
//  - override verifies that this method correctly implements the base interface.
//  - std::string_view avoids copying the character name.
//  - Trailing const means the strategy object is not modified by the attack.
class SwordAttack : public AttackBehavior {
public:
    void attack(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " attacks with a sword.\n";
    }
};


// Concrete spell-attack strategy implementing AttackBehavior.
class SpellAttack : public AttackBehavior {
public:
    void attack(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " casts a powerful spell.\n";
    }
};


class BowAttack : public AttackBehavior {
public:
    void attack(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " fires an arrow.\n";
    }
};


// Abstract strategy interface for special abilities.
// Notes:
//  - virtual + = 0 makes use() pure virtual, so each concrete ability
//    strategy must provide its own implementation.
//  - std::string_view is passed by value because it is a lightweight,
//    non-owning read-only view of the character name.
//  - Trailing const means using the ability does not modify the strategy object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class AbilityBehavior {
public:
    virtual ~AbilityBehavior() = default;

    virtual void use(std::string_view character_name) const = 0;
};


// Concrete special-ability strategy implementing AbilityBehavior. It can be used 
// wherever an AbilityBehavior is expected, supporting runtime polymorphism and 
// Liskov substitution.
// Note:
//  - override verifies that the base interface is implemented correctly.
class ShieldBlock : public AbilityBehavior {
public:
    void use(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " blocks with a shield.\n";
    }
};


// Concrete teleport ability strategy implementing AbilityBehavior.
class Teleport : public AbilityBehavior {
public:
    void use(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " teleports to another position.\n";
    }
};


// Concrete quick-shot ability strategy implementing AbilityBehavior.
class QuickShot : public AbilityBehavior {
public:
    void use(std::string_view character_name) const override
    {
        std::cout << character_name
                  << " performs a quick shot.\n";
    }
};


// Character is composed of interchangeable behaviors instead
// of inheriting attack/ability implementations.
class Character {
private:
    std::string name_;

    // Character exclusively owns its current behaviors.
    // Notes:
    //  - Store behaviors through base-class smart pointers so concrete strategies
    //    can be selected and replaced at runtime.
    //  - unique_ptr expresses exclusive ownership: Character owns its behaviors
    //    and automatically destroys the old strategy when it is replaced.
    std::unique_ptr<AttackBehavior> attack_behavior_;
    std::unique_ptr<AbilityBehavior> ability_behavior_;

public:
    // Constructor takes ownership of the character's name and behavior strategies.
    // Notes:
    //  - name is taken by value, then moved into name_ to avoid an extra copy.
    //  - unique_ptr parameters are also taken by value because Character assumes
    //    ownership of the supplied strategies; std::move transfers that ownership
    //    into the member variables.
    //  - Validation ensures every Character has a valid name and both required behaviors.
    Character(
        std::string name,
        std::unique_ptr<AttackBehavior> attack_behavior,
        std::unique_ptr<AbilityBehavior> ability_behavior
    )
        : name_(std::move(name)),
          attack_behavior_(std::move(attack_behavior)),
          ability_behavior_(std::move(ability_behavior))
    {
        if (name_.empty()) {
            throw std::invalid_argument(
                "Character name cannot be empty."
            );
        }

        if (!attack_behavior_) {
            throw std::invalid_argument(
                "Character requires an attack behavior."
            );
        }

        if (!ability_behavior_) {
            throw std::invalid_argument(
                "Character requires an ability behavior."
            );
        }
    }

    // Return name_ by const reference to avoid copying the string
    // and prevent callers from modifying it through the getter.
    // Notes:
    //  - Trailing const means this method does not modify the Character object.
    //  - noexcept is appropriate because returning the member reference cannot throw.
    const std::string& name() const noexcept
    {
        return name_;
    }

    // Delegate the attack to the currently selected AttackBehavior strategy.
    // Notes:
    //  - -> accesses the strategy through the owned smart pointer.
    //  - Because attack() is virtual, the concrete strategy implementation
    //    (e.g. SwordAttack, SpellAttack, BowAttack) is selected at runtime.
    //  - Trailing const means Character itself is not modified by this call.
    void attack() const
    {
        attack_behavior_->attack(name_);
    }

    // Delegate the special ability to the currently selected AbilityBehavior 
    // strategy.
    // Notes:
    //  - -> accesses the strategy through the owned smart pointer.
    //  - Because use() is virtual, the concrete ability implementation
    //    (e.g. ShieldBlock, Teleport, QuickShot) is selected at runtime.
    //  - Trailing const means Character itself is not modified by this call.
    void use_ability() const
    {
        ability_behavior_->use(name_);
    }

    // Replace the current attack strategy at runtime.
    // Notes: 
    //  - Take unique_ptr by value because Character assumes ownership of the new 
    //    strategy.
    //  - Validate before replacing the existing behavior so the Character never 
    //    stores null.
    //  - std::move transfers ownership into attack_behavior_; the previously owned 
    //    strategy is then destroyed automatically.
    void set_attack_behavior(
        std::unique_ptr<AttackBehavior> attack_behavior
    )
    {
        if (!attack_behavior) {
            throw std::invalid_argument(
                "Attack behavior cannot be null."
            );
        }

        attack_behavior_ = std::move(attack_behavior);
    }

    // Replace the current ability strategy at runtime.
    // Notes:
    //  - Take unique_ptr by value because Character assumes ownership of the new 
    //    ability.
    //  - Validate before replacing the existing behavior so the Character never 
    //    stores null.
    //  - std::move transfers ownership into ability_behavior_;
    //    the previously owned ability strategy is destroyed automatically.
    void set_ability_behavior(
        std::unique_ptr<AbilityBehavior> ability_behavior
    )
    {
        if (!ability_behavior) {
            throw std::invalid_argument(
                "Ability behavior cannot be null."
            );
        }

        ability_behavior_ = std::move(ability_behavior);
    }
};


int main()
{
    try {
        // Warrior = Character composed with sword + shield behavior.
        Character warrior{
            "Warrior",
            std::make_unique<SwordAttack>(),
            std::make_unique<ShieldBlock>()
        };

        // Mage = Character composed with spell + teleport behavior.
        Character mage{
            "Mage",
            std::make_unique<SpellAttack>(),
            std::make_unique<Teleport>()
        };

        // Archer = Character composed with bow + quick-shot behavior.
        Character archer{
            "Archer",
            std::make_unique<BowAttack>(),
            std::make_unique<QuickShot>()
        };

        warrior.attack();
        warrior.use_ability();

        mage.attack();
        mage.use_ability();

        archer.attack();
        archer.use_ability();

        std::cout << '\n';

        // Strategy Pattern: behavior can be replaced at runtime.
        warrior.set_attack_behavior(
            std::make_unique<BowAttack>()
        );

        warrior.attack();
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
