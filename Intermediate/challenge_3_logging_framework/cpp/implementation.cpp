#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

// Small logging interface.
// It contains only behavior that every logger actually supports.
class Logger {
public:
    // Virtual destructor because Logger is used polymorphically.
    virtual ~Logger() = default;

    // Every concrete logger must provide its own logging implementation.
    // Notes:
    //  - std::string_view is a lightweight, non-owning view of string data.
    //  - It does not copy or own the characters and is usually passed by value.
    //  - Use it for read-only text that is only needed temporarily.
    //  - The referenced string data must remain alive while the view is used.
    virtual void log(std::string_view message) = 0;
};


// Logs messages to the console.
// Notes:
//  - Concrete Logger implementation for console output.
//  - string_view is passed by value because it is lightweight.
//  - override verifies the base-class method is implemented correctly.
class ConsoleLogger : public Logger {
public:
    void log(std::string_view message) override
    {
        std::cout << "[Console] "
                  << message
                  << '\n';
    }
};


// Logs messages to a file.
class FileLogger : public Logger {
private:
    // ofstream owns and manages the file resource using RAII.
    std::ofstream file_;

public:
    // Notes:
    //  - Single-argument constructor: explicit prevents implicit conversion.
    //  - Pass filename by const reference to avoid copying while keeping it 
    //    read-only.
    //  - std::string_view is not used here because file-opening APIs expect 
    //    an actual string/path representation
    //  - std::filesystem::path would be another idiomatic option.
    //  - Initialize the ofstream directly with a member initializer list.
    //  - std::ofstream manages the file resource automatically through RAII.
    explicit FileLogger(const std::string& filename)
        : file_(filename)
    {
        if (!file_) {
            throw std::runtime_error(
                "Could not open log file."
            );
        }
    }

    void log(std::string_view message) override
    {
        file_ << "[File] "
              << message
              << '\n';
    }
};


// Simulates logging to a remote server.
class RemoteLogger : public Logger {
private:
    std::string endpoint_;

public:
    // Notes:
    // - Take endpoint by value because RemoteLogger needs to store and own it
    //   for future log calls, independent of the caller's string lifetime.
    // - Move the local string into endpoint_ to avoid an additional copy.
    explicit RemoteLogger(std::string endpoint)
        : endpoint_(std::move(endpoint))
    {
        if (endpoint_.empty()) {
            throw std::invalid_argument(
                "Remote endpoint cannot be empty."
            );
        }
    }

    void log(std::string_view message) override
    {
        std::cout << "[Remote -> "
                  << endpoint_
                  << "] "
                  << message
                  << '\n';
    }
};


// Application depends only on the Logger abstraction.
// The logger is injected through the constructor.
class Application {
private:
    // Non-owning reference: Application uses an existing Logger whose
    // lifetime is managed elsewhere.
    // Notes:
    //  - Store Logger as a non-owning reference because Application requires
    //    a valid logger but does not manage its lifetime.
    //  - A pointer would also be possible, but could be nullptr and require 
    //    null checks.
    Logger& logger_;

public:
    explicit Application(Logger& logger)
        : logger_(logger)
    {
    }

    void run()
    {
        logger_.log("Application started.");
        logger_.log("Application is running.");
        logger_.log("Application finished.");
    }
};


int main()
{
    try {
        ConsoleLogger console_logger;
        Application console_app{console_logger};

        console_app.run();

        std::cout << '\n';

        FileLogger file_logger{"application.log"};
        Application file_app{file_logger};

        file_app.run();

        std::cout << '\n';

        RemoteLogger remote_logger{
            "https://logging.example.com"
        };

        Application remote_app{remote_logger};

        remote_app.run();
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
