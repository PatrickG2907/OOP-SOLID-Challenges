// ============================================================================
// Architecture Overview
// ============================================================================

//  - Applicant represents the loan applicant and stores the financial data
//    required by approval rules.
//  - Specification defines the polymorphic interface for loan-approval rules.
//  - CreditScoreSpecification, IncomeSpecification, and DebtRatioSpecification
//    each implement one focused approval rule.
//  - AndSpecification composes multiple Specification objects and requires
//    every contained rule to be satisfied.
//  - AndSpecification owns its rules through std::unique_ptr, allowing
//    different concrete specifications to be stored polymorphically.
//  - LoanApprovalService depends only on the Specification abstraction and
//    delegates the approval decision to the supplied specification.
//  - main() demonstrates the system by composing several approval rules and
//    evaluating multiple applicants against the same combined specification.
//
// Patterns / Concepts:
//  - Specification Pattern: approval criteria are represented as independent
//    specification objects.
//  - Polymorphism: concrete rules are evaluated through the Specification
//    interface.
//  - Composition: AndSpecification combines multiple specifications into one
//    reusable approval rule.


// ============================================================================
// Includes
// ============================================================================

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


// ============================================================================
// Domain Model
// ============================================================================

// Represents a loan applicant evaluated by approval specifications.
// Notes:
//  - name_ stores the applicant's owned name.
//  - annual_income_cents_ stores annual income in integer cents.
//  - existing_debt_cents_ stores existing debt in integer cents.
//  - credit_score_ stores the applicant's credit score.
//  - Validation here only ensures structurally valid applicant data.
//  - Approval thresholds are intentionally not enforced by Applicant;
//    they belong to the Specification implementations.
class Applicant {
private:
    std::string name_;
    std::int64_t annual_income_cents_;
    std::int64_t existing_debt_cents_;
    int credit_score_;
    
    // Validate that the applicant name is not empty.
    // Note:
    //  - static is appropriate because the check depends only on the supplied value.
    static void validate_name(std::string_view name) {
        if (name.empty()) {
            throw std::invalid_argument("Name must not be empty!");
        }
    }
    
    // Validate that annual income is not negative.
    static void validate_income_cents(std::int64_t income_cents) {
        if (income_cents < 0) {
            throw std::invalid_argument("Income must not be negative!");
        }
    }
    
    // Validate that existing debt is not negative.
    static void validate_debt_cents(std::int64_t debt_cents) {
        if (debt_cents < 0) {
            throw std::invalid_argument("Debt must not be negative!");
        }
    }
    
    // Validate that the credit score is not negative.
    // Note:
    //  - Whether the score is high enough for approval is handled by a 
    //    specification.
    static void validate_credit_score(int credit_score) {
        if (credit_score < 0) {
            throw std::invalid_argument("Credit score must not be negative!");
        }
    }
    
public:
    // Construct an Applicant with the financial data used by loan rules.
    // Notes:
    //  - name is taken by value because Applicant owns its own string.
    //  - std::move transfers the local string into name_.
    //  - Numeric values are small scalar types and are copied.
    //  - Validation is performed on the stored members after initialization.
    Applicant(
        std::string name,
        std::int64_t annual_income_cents,
        std::int64_t existing_debt_cents,
        int credit_score
    )
        : name_(std::move(name)),
          annual_income_cents_(annual_income_cents),
          existing_debt_cents_(existing_debt_cents),
          credit_score_(credit_score)
    {
        validate_name(name_);
        validate_income_cents(annual_income_cents_);
        validate_debt_cents(existing_debt_cents_);
        validate_credit_score(credit_score_);
    }
    
    // Provide read-only access to the applicant name without copying it.
    const std::string& name() const noexcept {
        return name_;
    }
    
    // Return the applicant's annual income in cents.
    std::int64_t annual_income_cents() const noexcept {
        return annual_income_cents_;
    }
    
    // Return the applicant's existing debt in cents.
    std::int64_t existing_debt_cents() const noexcept {
        return existing_debt_cents_;
    }
    
    // Return the applicant's credit score.
    int credit_score() const noexcept {
        return credit_score_;
    }
};


// ============================================================================
// Specification Interface
// ============================================================================

// Abstract specification interface for evaluating loan-approval rules.
// Notes:
//  - Concrete specifications implement one approval rule each.
//  - is_satisfied_by() evaluates the supplied Applicant and returns true
//    when the applicant satisfies that rule.
//  - Applicant is passed by const reference because specifications only inspect it.
//  - is_satisfied_by() is const because evaluating a rule should not modify
//    the specification object's state.
//  - The virtual destructor ensures safe destruction through Specification pointers.
class Specification {
public:
    virtual ~Specification() = default;
    
    // Evaluate whether the applicant satisfies this specification.
    virtual bool is_satisfied_by(const Applicant& applicant) const = 0;
};


// ============================================================================
// Concrete Specifications
// ============================================================================

// Concrete specification that checks whether an applicant meets a minimum
// credit-score requirement.
// Notes:
//  - Inherits from Specification and implements one focused approval rule.
//  - minimum_score_ stores the configured minimum acceptable credit score.
//  - validate_minimum_score() ensures the configured threshold is not negative.
//  - The constructor is explicit to prevent unintended implicit conversion
//    from int to CreditScoreSpecification.
//  - is_satisfied_by() compares the applicant's credit score with the configured
//    minimum and returns true when the requirement is met.
class CreditScoreSpecification : public Specification {
private:
    int minimum_score_;
    
    // Validate that the configured minimum credit score is not negative.
    // Note:
    //  - static is appropriate because the check depends only on the supplied value.
    static void validate_minimum_score(int minimum_score) {
        if (minimum_score < 0) {
            throw std::invalid_argument("Minimum credit score must not be negative!");
        }
    }
    
public:
    // Construct the specification with the required minimum credit score.
    explicit CreditScoreSpecification(int minimum_score)
        : minimum_score_(minimum_score)
    {
        validate_minimum_score(minimum_score_);
    }
    
    // Return whether the applicant meets or exceeds the minimum credit score.
    bool is_satisfied_by(const Applicant& applicant) const override {
        return applicant.credit_score() >= minimum_score_;
    }
};


// Concrete specification that checks whether an applicant meets a minimum
// annual-income requirement.
// Notes:
//  - Inherits from Specification and implements one focused approval rule.
//  - minimum_income_cents_ stores the configured minimum annual income
//    in integer cents.
//  - validate_minimum_income_cents() ensures the configured threshold is
//    not negative.
//  - The constructor is explicit to prevent unintended implicit conversion
//    from std::int64_t to IncomeSpecification.
//  - is_satisfied_by() compares the applicant's annual income with the
//    configured minimum and returns true when the requirement is met.
class IncomeSpecification : public Specification {
private:
    std::int64_t minimum_income_cents_;
    
    // Validate that the configured minimum annual income is not negative.
    // Note:
    //  - static is appropriate because the check depends only on the supplied value.
    static void validate_minimum_income_cents(std::int64_t minimum_income_cents) {
        if (minimum_income_cents < 0) {
            throw std::invalid_argument("Minimum income must not be negative!");
        }
    }
    
public:
    // Construct the specification with the required minimum annual income.
    explicit IncomeSpecification(std::int64_t minimum_income_cents)
        : minimum_income_cents_(minimum_income_cents)
    {
        validate_minimum_income_cents(minimum_income_cents_);
    }
    
    // Return whether the applicant meets or exceeds the minimum annual income.
    bool is_satisfied_by(const Applicant& applicant) const override {
        return applicant.annual_income_cents() >= minimum_income_cents_;
    }
};


// Concrete specification that checks whether an applicant's debt-to-income
// ratio stays within a configured maximum.
// Notes:
//  - Inherits from Specification and implements one focused approval rule.
//  - maximum_debt_ratio_ stores the highest allowed debt-to-income ratio.
//  - validate_maximum_debt_ratio() ensures the configured threshold is not
//    negative.
//  - The constructor is explicit to prevent unintended implicit conversion
//    from double to DebtRatioSpecification.
//  - is_satisfied_by() compares existing debt with annual income.
//  - Applicants with zero annual income automatically fail this specification
//    to avoid division by zero.
//  - The debt ratio is calculated using floating-point division so fractional
//    ratios are preserved.
class DebtRatioSpecification : public Specification {
private:
    double maximum_debt_ratio_;
    
    // Validate that the configured maximum debt ratio is not negative.
    // Note:
    //  - static is appropriate because the check depends only on the supplied value.
    static void validate_maximum_debt_ratio(double maximum_debt_ratio) {
        if (maximum_debt_ratio < 0) {
            throw std::invalid_argument("Maximum debt ratio must not be negative!"); 
        }
    }
    
public:
    // Construct the specification with the maximum allowed debt-to-income ratio.
    explicit DebtRatioSpecification(double maximum_debt_ratio) 
        : maximum_debt_ratio_(maximum_debt_ratio)
    {
        validate_maximum_debt_ratio(maximum_debt_ratio_);
    }
    
    // Return whether the applicant's debt-to-income ratio is within the limit.
    bool is_satisfied_by(const Applicant& applicant) const override {
        if (applicant.annual_income_cents() == 0) {
            return false;
        }
        
        const double debt_ratio = 
            static_cast<double>(applicant.existing_debt_cents()) / 
            static_cast<double>(applicant.annual_income_cents());
            
        return debt_ratio <= maximum_debt_ratio_;
    }
};


// ============================================================================
// Composite Specifications
// ============================================================================

// Composite specification that requires all contained specifications to pass.
// Notes:
//  - Inherits from Specification, so it can be used anywhere a single
//    Specification is expected.
//  - specifications_ owns the composed rules through std::unique_ptr.
//  - Different concrete specification types can be stored polymorphically.
//  - add() transfers ownership of a specification into the composite.
//  - Null specifications are rejected to keep the collection valid.
//  - An empty AndSpecification returns false by design so that no applicant
//    is approved when no approval rules are configured.
//  - is_satisfied_by() short-circuits and returns false as soon as one
//    contained specification fails.
//  - Returning true means every contained specification was satisfied.
//  - Because AndSpecification is itself a Specification, composites can be nested.
class AndSpecification : public Specification {
private:
    std::vector<std::unique_ptr<Specification>> specifications_;

public:
    // Add one owned specification to the composite.
    // Note:
    //  - The parameter is taken by value because ownership is transferred
    //    into specifications_ using std::move.
    void add(std::unique_ptr<Specification> specification) {
        if (!specification) {
            throw std::invalid_argument("Specification must not be null!");
        }
        specifications_.push_back(std::move(specification));
    }
    
    // Return whether the applicant satisfies every contained specification.
    bool is_satisfied_by(const Applicant& applicant) const override {
        // An empty rule set cannot approve an applicant in this domain.
        if (specifications_.empty()) {
            return false;
        }
        // Stop immediately when any approval rule fails.
        for (const auto& specification : specifications_) {
            if (!specification->is_satisfied_by(applicant)) {
                return false;
            }
        }
        return true;
    }
};


// ============================================================================
// Loan Approval Service
// ============================================================================

// Application service that evaluates loan applicants against approval rules.
// Notes:
//  - LoanApprovalService does not implement approval rules itself.
//  - It depends only on the Specification abstraction, not on concrete rule types.
//  - Applicant and Specification are passed by const reference because the
//    service only inspects them and does not take ownership.
//  - approve() delegates the decision to Specification::is_satisfied_by().
//  - approve() is const because the service does not modify its own state.
//  - This keeps the service independent of how individual rules are implemented
//    or composed.
class LoanApprovalService {
public:
    // Return whether the applicant satisfies the supplied approval specification.
    bool approve(
        const Applicant& applicant, 
        const Specification& specification
    ) const {
        return specification.is_satisfied_by(applicant);
    }
};


// ============================================================================
// Demo / Entry Point
// ============================================================================

// Demonstrate the loan approval system with several applicant profiles.
// Notes:
//  - LoanApprovalService evaluates applicants against a supplied specification.
//  - The applicants are chosen to exercise different approval outcomes.
//  - AndSpecification combines the individual approval rules into one rule set.
//  - Concrete specifications are created dynamically and owned by the composite.
//  - The same composed specification is reused for every applicant.
//  - Each applicant is evaluated and the resulting approval status is printed.
int main() {
    const LoanApprovalService loan_approval_service;
    
    // Create applicants with different financial profiles.
    const std::array<Applicant, 5> applicants{
        Applicant{"Clearly approvable", 8'000'000, 1'000'000, 760},
        Applicant{"Fails credit score", 9'000'000, 1'500'000, 620},
        Applicant{"Fails income", 3'500'000, 500'000, 740},
        Applicant{"Fails debt-ratio", 7'000'000, 3'500'000, 750},
        Applicant{"Edge case: zero income", 0, 10'000, 800}
    };
    
    // Compose the complete loan-approval rule set.
    AndSpecification and_specification;
    
    // Require a minimum credit score of 700.
    and_specification.add(std::make_unique<CreditScoreSpecification>(700));
    
    // Require a minimum annual income of 50,000 currency units.
    and_specification.add(std::make_unique<IncomeSpecification>(5'000'000));
    
    // Allow a maximum debt-to-income ratio of 40%.
    and_specification.add(std::make_unique<DebtRatioSpecification>(0.4));
    
    // Evaluate every applicant against the same composed approval rules.
    for (const Applicant& applicant : applicants) {
        std::string_view result = "rejected";
        if (loan_approval_service.approve(applicant, and_specification)) {
            result = "approved";
        }
        std::cout << applicant.name() << ": " << result << '\n';
    }
    
    return 0;
}
