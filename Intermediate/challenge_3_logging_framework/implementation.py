from abc import ABC, abstractmethod
from typing import List

# -------------------------------
# Step 1: Define Logger Strategy Interface
# -------------------------------
class LoggerStrategy(ABC):
    """OOP: Abstract base class for all loggers (polymorphism)."""
    @abstractmethod
    def log(self, message: str) -> None:
        """Method every concrete logger must implement (SRP + LSP)."""
        pass

# -------------------------------
# Step 2: Concrete Loggers
# -------------------------------
class ConsoleLogger(LoggerStrategy):
    """Concrete logger for console output (SRP: only handles console logging)."""
    def log(self, message: str) -> None:
        print(f"[Console] {message}")

class FileLogger(LoggerStrategy):
    """Concrete logger for file output (SRP: only handles file writing)."""
    def __init__(self, filename: str):
        self.filename = filename

    def log(self, message: str) -> None:
        with open(self.filename, "a") as f:
            f.write(f"{message}\n")

class RemoteLogger(LoggerStrategy):
    """Concrete logger for remote server (SRP: only handles remote logging)."""
    def log(self, message: str) -> None:
        # Simulate sending to remote server
        print(f"[Remote] Sending '{message}' to server...")

# -------------------------------
# Step 3: Logger Class (aggregates strategies)
# -------------------------------
class Logger:
    """OOP: This class manages multiple logging strategies."""
    def __init__(self, strategies: List[LoggerStrategy] = None):
        # DIP: depends on abstraction (LoggerStrategy), not concrete classes
        self.strategies = strategies or []

    def log_message(self, message: str):
        """Logs message to all strategies (SRP: delegation only)."""
        self._validate_message(message)
        for strategy in self.strategies:
            strategy.log(message)  # Polymorphism in action
            
    def _validate_message(self, message: str):
        if not message:
            raise ValueError("Message cannot be empty!")

    def add_strategy(self, strategy: LoggerStrategy):
        """OCP: You can extend functionality without modifying existing code."""
        self.strategies.append(strategy)

    def remove_strategy(self, strategy: LoggerStrategy):
        if strategy in self.strategies:
            self.strategies.remove(strategy)

# -------------------------------
# Step 4: Usage
# -------------------------------
remote_logger = RemoteLogger()
console_logger = ConsoleLogger()
file_logger = FileLogger("log.txt")

logger = Logger([console_logger, remote_logger])  # Dependency Injection
logger.log_message("Hello World!")  # Logs to console and remote

logger.add_strategy(file_logger)  # Open/Closed: extend logging without changing Logger
logger.log_message("Logging to all three targets")

logger.remove_strategy(remote_logger)
logger.log_message("Now only console and file")  # Dynamic strategy removal
