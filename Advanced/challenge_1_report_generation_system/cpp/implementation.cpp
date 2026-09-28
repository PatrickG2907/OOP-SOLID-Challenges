#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


// Represents the data that should appear in a report.
class Report {
private:
    // Notes:
    //  - Private member variables use trailing underscores for distinction.
    //  -  std::explicitly qualifies standard-library types without importing
    //     the entire std namespace.
    std::string title_;
    std::vector<std::string> lines_;

public:
    // Notes:
    //  - explicit prevents unintended implicit conversion from std::string to Report.
    //  - Take title by value, then move it into title_ to avoid an extra copy.
    //  - The member initializer list initializes the member directly.
    //  - Validate the title so every Report starts in a valid state.
    explicit Report(std::string title)
        : title_(std::move(title))
    {
        if (title_.empty()) {
            throw std::invalid_argument(
                "Report title cannot be empty."
            );
        }
    }

    // Notes:
    //  - Take line by value so the function receives its own local string.
    //  - After validation, move the local string into the vector to avoid
    //    an additional copy. The vector then owns its own stored string.
    void add_line(std::string line)
    {
        if (line.empty()) {
            throw std::invalid_argument(
                "Report line cannot be empty."
            );
        }

        lines_.push_back(std::move(line));
    }

    // Notes:
    //  - Return title_ by const reference to avoid copying the string
    //    and prevent callers from modifying it through the returned reference.
    //  - Trailing const means this getter does not modify the Report object.
    //  - noexcept guarantees that the getter does not throw.
    const std::string& title() const noexcept
    {
        return title_;
    }

    // Notes:
    //  - Return the vector by const reference to avoid copying all stored strings
    //    and prevent callers from modifying the collection through this getter.
    //  - Trailing const means the Report object is not modified.
    //  - noexcept is appropriate because returning the member reference cannot throw.
    const std::vector<std::string>& lines() const noexcept
    {
        return lines_;
    }
};


// Strategy interface for generating reports.
// Notes:
//  - Abstract base class for report-generation strategies.
//  - virtual + = 0 makes generate() pure virtual, so derived classes must
//    provide their own implementation and the base class cannot be instantiated.
//  - Report and filename are passed by const reference to avoid unnecessary copies
//    and prevent modification of the input objects.
//  - Trailing const means generate() does not modify the strategy object itself.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class ReportStrategy {
public:
    virtual ~ReportStrategy() = default;

    virtual void generate(
        const Report& report,
        const std::string& filename
    ) const = 0;
};


// Concrete strategy for CSV reports.
class CsvReport : public ReportStrategy {
private:
    // Escape text according to basic CSV quoting rules.
    // Notes:
    //  - Private static helper for preparing a string as a valid CSV field.
    //  - Static is appropriate because the function does not depend on any
    //    CsvReport instance state; it operates only on the provided argument.
    //  - The input is passed by const reference to avoid copying and prevent
    //    modification.
    //  - A range-based for loop processes each character in the string.
    //  - Ordinary characters are copied as-is, while embedded double quotes are 
    //    doubled, which is the CSV escaping rule for quotes inside quoted fields.
    //  - The result starts and ends with a double quote so commas, quotes,
    //    or line breaks inside the value are treated as part of one CSV field.
    static std::string escape_csv(const std::string& value)
    {
        std::string escaped{"\""};

        for (char character : value) {
            if (character == '"') {
                escaped += "\"\"";
            }
            else {
                escaped += character;
            }
        }

        escaped += '"';

        return escaped;
    }

public:
    // Concrete implementation of ReportStrategy::generate().
    // Notes:
    //  - Report and filename are passed by const reference to avoid unnecessary 
    //    copies.
    //  - Trailing const means the CsvReport object is not modified.
    //  - override verifies the base-class method is implemented correctly.
    //  - Construct the output file directly and check that it opened successfully.
    //  - std::ofstream manages the file through RAII and closes it automatically.
    //  - Iterate through report lines by const reference to avoid copying,
    //    escape each CSV field, and write one row per report line.
    void generate(
        const Report& report,
        const std::string& filename
    ) const override
    {
        std::ofstream file{filename};

        if (!file) {
            throw std::runtime_error(
                "Could not create CSV report."
            );
        }

        file << "Title,Content\n";

        for (const std::string& line : report.lines()) {
            file << escape_csv(report.title())
                 << ','
                 << escape_csv(line)
                 << '\n';
        }
    }
};


// Concrete strategy for PDF reports.
// Note: It follows the same interface as CsvReport, so ReportGenerator can use either
// implementation polymorphically without knowing the concrete format.
class PdfReport : public ReportStrategy {
public:
    void generate(
        const Report& report,
        const std::string& filename
    ) const override
    {
        std::ofstream file{filename};

        if (!file) {
            throw std::runtime_error(
                "Could not create PDF report."
            );
        }

        // Simulated PDF output.
        file << "=== PDF REPORT ===\n\n";
        file << report.title() << "\n\n";

        for (const std::string& line : report.lines()) {
            file << line << '\n';
        }
    }
};


// Context class that delegates report generation to a strategy.
class ReportGenerator {
private:
    // Non-owning pointer allows the strategy to be changed later (unlike a reference 
    // member, a pointer can be reseated).
    // The constructor/setter accept references because a valid strategy is required;
    // internally, a pointer is stored so the strategy can later be replaced.
    ReportStrategy* strategy_;

public:
    // Constructor.
    // Notes:
    //  - Accept the constructor dependency by reference because a valid strategy
    //    is required, then store its address in the pointer member.
    //  - ReportGenerator does not own the strategy, so the strategy must outlive it.
    explicit ReportGenerator(ReportStrategy& strategy)
        : strategy_(&strategy)
    {
    }

    // Replace the currently used strategy by storing the address of another
    // valid ReportStrategy object. 
    // Notes:
    //  - The reference parameter prevents nullptr, while the pointer member can be 
    //    reseated.
    //  - noexcept is appropriate because assigning an address cannot throw.
    void set_strategy(ReportStrategy& strategy) noexcept
    {
        strategy_ = &strategy;
    }

    // Delegate report generation to the currently selected strategy.
    // Notes:
    //  - -> accesses the object through the stored pointer.
    //  - Because generate() is virtual, the concrete implementation
    //    (e.g. CsvReport or PdfReport) is selected at runtime.
    //  - ReportGenerator therefore depends only on the ReportStrategy abstraction.
    void generate(
        const Report& report,
        const std::string& filename
    ) const
    {
        strategy_->generate(report, filename);
    }
};


int main()
{
    try {
        Report report{"Monthly Sales Report"};

        report.add_line("Revenue: 125000");
        report.add_line("Expenses: 80000");
        report.add_line("Profit: 45000");

        PdfReport pdf_strategy;
        CsvReport csv_strategy;

        // Dependency injection:
        // ReportGenerator receives its strategy from outside.
        ReportGenerator generator{pdf_strategy};

        generator.generate(
            report,
            "sales_report.pdf.txt"
        );

        // Switch strategy at runtime.
        generator.set_strategy(csv_strategy);

        generator.generate(
            report,
            "sales_report.csv"
        );

        std::cout << "Reports generated successfully.\n";
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
