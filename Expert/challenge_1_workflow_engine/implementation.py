from abc import ABC, abstractmethod
from typing import Dict, Optional

# ===========================
# STEP INTERFACE (OOP + SOLID)
# ===========================
# Open/Closed: New steps can be added without modifying engine.
# Dependency Inversion: Engine depends on abstract Step, not concrete implementations.
class Step(ABC):
    @abstractmethod
    def execute(self, context: dict) -> str:
        """
        Executes the step logic.
        Returns a key for the next step (for branching).
        """
        pass

# ===========================
# CONCRETE STEPS (OOP)
# ===========================
class StepA(Step):
    def execute(self, context: dict) -> str:
        print("StepA: validating data")
        # Example: branching logic
        if context.get("data_valid", False):
            return "success"
        return "fail"

class StepB(Step):
    def execute(self, context: dict) -> str:
        print("StepB: processing data")
        return "done"

class StepC(Step):
    def execute(self, context: dict) -> str:
        print("StepC: handling failure")
        return "done"

# ===========================
# FLOW ENGINE (OOP + SOLID)
# ===========================
# Single Responsibility: Engine only manages flow.
# Open/Closed: Works with any Step implementation.
# Dependency Inversion: Works with Step interface.
class FlowEngine:
    def __init__(self):
        # Mapping: step instance -> {result_key: next_step_instance}
        self.flow_map: Dict[Step, Dict[str, Optional[Step]]] = {}
        self.start_step: Optional[Step] = None

    def set_start(self, step: Step):
        self.start_step = step

    def add_transition(self, from_step: Step, result_key: str, to_step: Optional[Step]):
        if from_step not in self.flow_map:
            self.flow_map[from_step] = {}
        self.flow_map[from_step][result_key] = to_step

    def run(self, context: dict):
        current_step = self.start_step
        while current_step:
            result = current_step.execute(context)
            next_step = self.flow_map.get(current_step, {}).get(result)
            current_step = next_step

# ===========================
# CLIENT / CONFIGURATION
# ===========================
# Shows Open/Closed: we can add new steps and wiring without touching engine.
context = {"data_valid": True}

step_a = StepA()
step_b = StepB()
step_c = StepC()

engine = FlowEngine()
engine.set_start(step_a)
engine.add_transition(step_a, "success", step_b)
engine.add_transition(step_a, "fail", step_c)
engine.add_transition(step_b, "done", None)
engine.add_transition(step_c, "done", None)

engine.run(context)
