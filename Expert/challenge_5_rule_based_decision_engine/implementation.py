from abc import ABC, abstractmethod
from dataclasses import dataclass, field

# ------------------------
# Applicant Dataclass with Validation
# ------------------------
@dataclass
class Applicant:
    loan_amount: float
    credit_score: int
    suspicious_activity: bool

    def __post_init__(self):
        if self.loan_amount <= 0:
            raise ValueError(f"Loan amount must be positive, got {self.loan_amount}")
        if not (0 <= self.credit_score <= 850):
            raise ValueError(f"Credit score must be between 0 and 850, got {self.credit_score}")
        if not isinstance(self.suspicious_activity, bool):
            raise TypeError(f"suspicious_activity must be a boolean, got {type(self.suspicious_activity).__name__}")

# ------------------------
# Rule Interface
# ------------------------
class Rule(ABC):
    @abstractmethod
    def is_satisfied(self, applicant: Applicant) -> bool:
        pass

# ------------------------
# Concrete Rules with Validation
# ------------------------
@dataclass
class LoanAmountRule(Rule):
    max_amount: float

    def __post_init__(self):
        if self.max_amount <= 0:
            raise ValueError(f"max_amount must be positive, got {self.max_amount}")

    def is_satisfied(self, applicant: Applicant) -> bool:
        return applicant.loan_amount <= self.max_amount

@dataclass
class CreditScoreRule(Rule):
    min_score: int

    def __post_init__(self):
        if not (0 <= self.min_score <= 850):
            raise ValueError(f"min_score must be between 0 and 850, got {self.min_score}")

    def is_satisfied(self, applicant: Applicant) -> bool:
        return applicant.credit_score >= self.min_score

@dataclass
class FraudCheckRule(Rule):
    # No parameters, so no validation needed
    def is_satisfied(self, applicant: Applicant) -> bool:
        return not applicant.suspicious_activity

# ------------------------
# Specification Pattern
# ------------------------
class AndSpecification(Rule):
    def __init__(self, *rules: Rule):
        self.rules = rules

    def is_satisfied(self, applicant: Applicant) -> bool:
        return all(rule.is_satisfied(applicant) for rule in self.rules)

# ------------------------
# Loan Approval Service
# ------------------------
class LoanApprovalService:
    def __init__(self, rule: Rule):
        self.rule = rule

    def approve(self, applicant: Applicant) -> bool:
        return self.rule.is_satisfied(applicant)

# ------------------------
# Example Usage
# ------------------------
if __name__ == "__main__":
    # Valid rules
    eligibility_rule = AndSpecification(
        LoanAmountRule(max_amount=50000),
        CreditScoreRule(min_score=650),
        FraudCheckRule()
    )

    service = LoanApprovalService(rule=eligibility_rule)

    applicant_1 = Applicant(loan_amount=40000, credit_score=700, suspicious_activity=False)
    print(service.approve(applicant_1))  # True

    # Invalid rule: negative max_amount
    try:
        bad_rule = LoanAmountRule(max_amount=-1000)
    except ValueError as e:
        print(e)  # max_amount must be positive, got -1000

    # Invalid rule: credit score too high
    try:
        bad_rule2 = CreditScoreRule(min_score=900)
    except ValueError as e:
        print(e)  # min_score must be between 0 and 850, got 900
