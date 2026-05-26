from abc import ABC, abstractmethod
from typing import List
import os

# ---------------------------
# STRATEGY INTERFACE
# ---------------------------
class ReportStrategy(ABC):
    """Abstract base class for all report types."""
    
    def __init__(self, name: str):
        if not name:
            raise ValueError("Report name cannot be empty.")
        self._name = name

    @property
    def name(self) -> str:
        return self._name

    @abstractmethod
    def generate_report(self, output_dir: str = ".") -> str:
        """Generate the report and return the file path."""
        pass

# ---------------------------
# CONCRETE REPORTS
# ---------------------------
class PdfReport(ReportStrategy):
    """Concrete PDF report."""

    def generate_report(self, output_dir: str = ".") -> str:
        filename = os.path.join(output_dir, f"{self.name}.pdf")
        # Simulate PDF generation
        with open(filename, "w") as f:
            f.write(f"PDF report content for {self.name}")
        return filename

class CsvReport(ReportStrategy):
    """Concrete CSV report."""

    def generate_report(self, output_dir: str = ".") -> str:
        filename = os.path.join(output_dir, f"{self.name}.csv")
        # Simulate CSV generation
        with open(filename, "w") as f:
            f.write(f"name,value\n{self.name},123\n")
        return filename

# ---------------------------
# CONTEXT / MANAGER
# ---------------------------
class ReportManager:
    """Manages a collection of reports."""
    
    def __init__(self):
        self._reports: List[ReportStrategy] = []

    def add_report(self, report: ReportStrategy):
        self._reports.append(report)

    def generate_all(self, output_dir: str = ".") -> List[str]:
        """Generate all reports and return list of generated file paths."""
        file_paths = []
        for report in self._reports:
            file_path = report.generate_report(output_dir)
            file_paths.append(file_path)
            print(f"Generated report: {file_path}")
        return file_paths

# ---------------------------
# USAGE EXAMPLE
# ---------------------------
if __name__ == "__main__":
    manager = ReportManager()
    manager.add_report(PdfReport("financial_q1"))
    manager.add_report(CsvReport("sales_q1"))

    # Generate all reports in current directory
    manager.generate_all()
