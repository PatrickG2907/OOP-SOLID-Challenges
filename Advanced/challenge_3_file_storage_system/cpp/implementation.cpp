#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>


// Abstract strategy interface for file storage.
// Notes:
//  - virtual + = 0 makes store() pure virtual, so each concrete storage
//    strategy must provide its own implementation.
//  - filename is passed by const reference to avoid copying and prevent modification.
//  - content uses std::string_view because it is a lightweight, non-owning,
//    read-only view that is only needed during the call.
//  - Trailing const means storing the file does not modify the strategy object itself.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class FileStorage {
public:
    virtual ~FileStorage() = default;

    virtual void store(
        const std::string& filename,
        std::string_view content
    ) const = 0;
};


// Concrete local-storage strategy implementing FileStorage.
// Notes:
//  - override verifies that store() correctly implements the base interface.
//  - Validate the filename before opening the file.
//  - std::ofstream opens the file during construction and manages it through RAII,
//    so the file is closed automatically when the stream goes out of scope.
//  - if (!file) checks whether opening/creating the file failed.
//  - Writing content through << stores the provided string_view in the file.
//  - Trailing const means LocalStorage itself is not modified by the operation.
class LocalStorage : public FileStorage {
public:
    void store(
        const std::string& filename,
        std::string_view content
    ) const override
    {
        if (filename.empty()) {
            throw std::invalid_argument(
                "Filename cannot be empty."
            );
        }

        std::ofstream file{filename};

        if (!file) {
            throw std::runtime_error(
                "Could not create local file."
            );
        }

        file << content;
    }
};


// Concrete cloud-storage strategy implementing FileStorage.
// Notes:
//  - endpoint_ stores the cloud destination owned by this object.
//  - explicit prevents unintended implicit conversion from std::string to CloudStorage.
//  - The endpoint is taken by value and moved into the member to avoid an extra copy.
//  - Constructor validation ensures the strategy is created with a valid endpoint.
//  - store() overrides the base interface, validates the filename, and simulates
//    an upload using the configured endpoint.
//  - std::string_view is used for content because it is only read during the call.
//  - Trailing const means storing the file does not modify the CloudStorage object.
class CloudStorage : public FileStorage {
private:
    std::string endpoint_;

public:
    explicit CloudStorage(std::string endpoint)
        : endpoint_(std::move(endpoint))
    {
        if (endpoint_.empty()) {
            throw std::invalid_argument(
                "Cloud endpoint cannot be empty."
            );
        }
    }

    void store(
        const std::string& filename,
        std::string_view content
    ) const override
    {
        if (filename.empty()) {
            throw std::invalid_argument(
                "Filename cannot be empty."
            );
        }

        std::cout << "Uploading file \""
                  << filename
                  << "\" to "
                  << endpoint_
                  << '\n';

        std::cout << "Content size: "
                  << content.size()
                  << " bytes\n";
    }
};


// Context class that delegates storage to the selected strategy.
// Notes:
//  - FileManager uses dependency injection: a FileStorage implementation is
//    supplied from outside instead of being created internally.
//  - The constructor accepts FileStorage by reference to preserve polymorphism,
//    avoid copying, and require a valid storage object.
//  - The class stores the address of that outside object in a non-owning pointer,
//    allowing the active strategy to be replaced later.
//  - Because the pointer is non-owning, the referenced FileStorage object must
//    remain alive while FileManager uses it.
//  - Delegating through the FileStorage abstraction supports runtime polymorphism,
//    the Strategy Pattern, and the Open/Closed Principle.
class FileManager {
private:
    // Non-owning pointer allows the storage strategy to be replaced.
    FileStorage* storage_;

public:
    explicit FileManager(FileStorage& storage)
        : storage_(&storage)
    {
    }

    void set_storage(FileStorage& storage) noexcept
    {
        storage_ = &storage;
    }

    void save(
        const std::string& filename,
        std::string_view content
    ) const
    {
        storage_->store(filename, content);
    }
};


int main()
{
    try {
        LocalStorage local_storage;

        CloudStorage cloud_storage{
            "https://storage.example.com"
        };

        // Dependency Injection:
        // FileManager receives its storage strategy from outside.
        FileManager manager{local_storage};

        manager.save(
            "example.txt",
            "This file is stored locally."
        );

        // Switch storage strategy at runtime.
        manager.set_storage(cloud_storage);

        manager.save(
            "backup.txt",
            "This file is uploaded to cloud storage."
        );
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
