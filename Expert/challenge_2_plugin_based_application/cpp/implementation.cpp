// ------------------------------------------------------------
// Architecture overview
// ------------------------------------------------------------
//
// ActionPlugin
//  - Defines the executable capability of a plugin.
//  - Concrete plugins implement execute() and return an owned std::string result.
//  - Runtime input is supplied as std::string_view and is not stored by the interface.
//
// ConfigurablePlugin
//  - Defines an optional configuration capability.
//  - Only plugins that actually support configuration implement this interface.
//  - Concrete plugins are responsible for validating their own configuration values.
//
// GreetingPlugin
//  - Implements ActionPlugin only.
//  - Demonstrates a simple action-only plugin with no persistent configuration.
//
// UppercasePlugin
//  - Implements both ActionPlugin and ConfigurablePlugin.
//  - Owns persistent prefix configuration and applies it when transforming input.
//  - Keeps configuration validation inside the concrete plugin.
//
// CoreApplication
//  - Owns installed plugins through std::unique_ptr<ActionPlugin>.
//  - Stores plugins by unique string ID in an unordered_map.
//  - Installs, removes, and executes plugins without depending on concrete types.
//  - Uses runtime capability detection to configure plugins that also implement
//    ConfigurablePlugin.
//
// Design objectives
//  - Composition over Inheritance:
//    CoreApplication gains behavior by owning plugin objects rather than being
//    subclassed for each plugin type.
//
//  - Open/Closed Principle:
//    New ActionPlugin implementations can be added without modifying CoreApplication.
//
//  - Interface Segregation:
//    Execution and configuration are separate capabilities, so plugins implement
//    only the interfaces they actually need.


#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

// ------------------------------------------------------------
// Plugin interfaces
// ------------------------------------------------------------

// Abstract interface for configurable plugins.
class ConfigurablePlugin {
public:
    // The virtual destructor allows safe destruction through the base interface.
    virtual ~ConfigurablePlugin() = default;

    // The pure virtual configure() method makes this class non-instantiable;
    // concrete derived classes must provide the actual configuration behavior.
    virtual void configure(std::string_view value) = 0;
};


// Abstract interface for executable plugins.
class ActionPlugin {
public:
    // The virtual destructor allows safe destruction through the base interface.
    virtual ~ActionPlugin() = default;

    // The pure virtual execute() method makes this class non-instantiable;
    // concrete derived classes must provide the actual plugin behavior.
    virtual std::string execute(std::string_view input) const = 0;
};


// ------------------------------------------------------------
// Concrete plugins
// ------------------------------------------------------------

// Concrete action-only plugin.
// Notes:
//  - Inherits from ActionPlugin and provides the required execute() implementation.
//  - input is received as std::string_view because it is read-only and only needed
//    during the call, avoiding an unnecessary input copy.
//  - execute() is const because running this plugin does not modify its state.
//  - override verifies that the ActionPlugin interface is implemented correctly.
//  - The result is built as an owning std::string so it can be safely returned
//    independently of the non-owning input string_view.
class GreetingPlugin : public ActionPlugin {
public:
    std::string execute(std::string_view input) const override {
        std::string result = "Output from GreetingPlugin: ";
        result += input;
        return result;
    }
};


// Concrete plugin that supports both execution and configuration.
// Notes:
//  - Inherits from ActionPlugin for executable behavior and ConfigurablePlugin
//    for runtime reconfiguration.
//  - validate_prefix() centralizes the prefix invariant so the same validation
//    rule is reused by both construction and later configuration changes.
//  - The helper is static because validation depends only on the supplied prefix
//    and does not require access to object state.
class UppercasePlugin : public ActionPlugin, public ConfigurablePlugin {
private:
    // prefix_ stores the plugin's persistent configuration as an owned std::string.
    std::string prefix_;
    
    // Validate a candidate prefix before it becomes part of the plugin state.
    // An empty prefix is rejected so every valid UppercasePlugin keeps
    // a non-empty prefix throughout its lifetime.
    static void validate_prefix(std::string_view prefix) {
        if (prefix.empty()) {
            throw std::invalid_argument("Prefix must not be empty!");
        }
    }
    
public:
    // Construct the plugin with its initial prefix configuration.
    // Notes:
    //  - prefix is received as std::string_view because it is only read here.
    //  - prefix_ owns its own std::string copy of the supplied value.
    //  - explicit prevents unintended implicit construction from string-like values.
    //  - Validation ensures the object starts in a valid state.
    explicit UppercasePlugin(std::string_view prefix) 
        : prefix_(prefix)
    {
        validate_prefix(prefix);
    }
    
    // Update the persistent prefix configuration.
    // Notes:
    //  - Validate before assignment so invalid input cannot overwrite
    //    the existing valid prefix.
    //  - override verifies the ConfigurablePlugin interface is implemented correctly.
    void configure(std::string_view value) override {
        validate_prefix(value);
        prefix_ = value;
    }
    
    // Transform the runtime input to uppercase and return it with the configured prefix.
    // Notes:
    //  - input is passed as std::string_view because it is read-only and only needed
    //    during this call.
    //  - execute() is const because execution does not modify plugin configuration.
    //  - override verifies the ActionPlugin interface is implemented correctly.
    //  - Each char is converted to unsigned char before std::toupper to avoid
    //    undefined behavior for negative signed-char values.
    //  - A new owning std::string is returned to the caller.
    std::string execute(std::string_view input) const override {
        std::string result = "[" + prefix_ + "]: ";
        for (char c : input) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            result += c;
        }
        return result;
    }
};


// ------------------------------------------------------------
// Core application
// ------------------------------------------------------------

// Core application responsible for managing and executing installed plugins.
// Notes:
//  - Plugins are stored through the ActionPlugin abstraction, so CoreApplication
//    does not depend on concrete plugin implementations.
//  - unique_ptr gives CoreApplication exclusive ownership of installed plugins.
//  - The unordered_map associates each plugin with a unique string ID.
//  - This design uses composition: CoreApplication gains behavior by owning plugins
//    rather than inheriting from plugin-specific application classes.
//  - New ActionPlugin implementations can be installed without changing
//    CoreApplication, supporting the Open/Closed Principle.
class CoreApplication {
private:
    // Store installed plugins by unique ID.
    // Each unique_ptr owns one concrete plugin polymorphically through ActionPlugin.
    std::unordered_map<std::string, std::unique_ptr<ActionPlugin>> plugins_;
    
    // Validate that a plugin ID is not empty.
    // static is appropriate because this validation depends only on the supplied ID
    // and does not require access to CoreApplication state.
    static void validate_id(std::string_view id) {
        if (id.empty()) {
            throw std::invalid_argument("Plugin ID must not be empty!");
        }
    }
    
    // Ensure that a plugin with the supplied ID is currently installed.
    // Notes:
    //  - find() performs a lookup without modifying the map.
    //  - The method is non-static because it depends on plugins_.
    //  - trailing const means validation does not modify CoreApplication.
    void validate_existing_id(const std::string& id) const {
        if (plugins_.find(id) == plugins_.end()) {
            throw std::invalid_argument("Plugin ID is not installed!");
        }
    }
    
    // Validate that the supplied unique_ptr owns a plugin object.
    // Notes:
    //  - Pass by const reference to inspect the unique_ptr without transferring
    //    ownership or attempting to copy it.
    //  - static is appropriate because the check does not depend on application state.
    static void validate_ptr(const std::unique_ptr<ActionPlugin>& ptr) {
        if (!ptr) {
            throw std::invalid_argument("Plugin must not be null!");
        }
    }
    
    // Ensure that the supplied ID is not already assigned to an installed plugin.
    // Notes:
    //  - Plugin IDs must be unique within CoreApplication.
    //  - The method is non-static because it checks plugins_.
    //  - trailing const means the lookup does not modify application state.
    void validate_unique_id(const std::string& id) const {
        if (plugins_.find(id) != plugins_.end()) {
            throw std::invalid_argument("Plugin ID is already installed!");
        }
    }
public:
    // Construct an application with no installed plugins.
    // plugins_ is default-constructed as an empty unordered_map.
    CoreApplication() = default;
    
    // Install a new executable plugin under a unique ID.
    // Notes:
    //  - id is taken by value because CoreApplication stores its own map key
    //    and can move the local string into the map.
    //  - plugin is taken by value because ownership is transferred to CoreApplication.
    //  - Validation occurs before either value is moved.
    //  - emplace() inserts a new entry without replacement semantics.
    void install_plugin(std::string id, std::unique_ptr<ActionPlugin> plugin) {
        validate_id(id);
        validate_ptr(plugin);
        validate_unique_id(id);
        
        plugins_.emplace(std::move(id), std::move(plugin));
    }
    
    // Remove an installed plugin by ID.
    // Notes:
    //  - The ID is passed by const reference to avoid copying.
    //  - Validation ensures the requested plugin exists before removal.
    //  - Erasing the map entry destroys its unique_ptr, which automatically
    //    destroys the owned concrete plugin.
    void remove_plugin(const std::string& id) {
        validate_id(id);
        validate_existing_id(id);
        
        plugins_.erase(id);
    }
    
    // Execute an installed plugin using runtime input and return its result.
    // Notes:
    //  - input is passed as std::string_view because it is read-only and only
    //    needed during this call.
    //  - at() retrieves an existing plugin without inserting a new map entry.
    //  - execute() is called polymorphically through the ActionPlugin interface.
    //  - trailing const is possible because neither CoreApplication nor the
    //    plugin's execute() operation modifies application state.
    std::string run_plugin(const std::string& id, std::string_view input) const {
        validate_id(id);
        validate_existing_id(id);
        
        return plugins_.at(id)->execute(input);
    }
    
    // Configure an installed plugin if it supports the ConfigurablePlugin capability.
    // Notes:
    //  - CoreApplication stores plugins as ActionPlugin objects, while configuration
    //    is an independent optional capability.
    //  - get() provides a temporary non-owning raw pointer while the unique_ptr
    //    inside plugins_ keeps ownership.
    //  - dynamic_cast performs a runtime cross-cast from ActionPlugin* to
    //    ConfigurablePlugin*.
    //  - The cast succeeds only when the concrete plugin implements both interfaces.
    //  - Configuration-value validation remains the responsibility of the
    //    concrete ConfigurablePlugin implementation.
    void configure_plugin(const std::string& id, std::string_view value) {
        validate_id(id);
        validate_existing_id(id);
        
        ActionPlugin* plugin = plugins_.at(id).get();
        
        ConfigurablePlugin* configurable =
            dynamic_cast<ConfigurablePlugin*>(plugin);

        if (!configurable) {
            throw std::invalid_argument(
                "Plugin does not support configuration!"
            );
        }

        configurable->configure(value);
    }
};


// ------------------------------------------------------------
// Demonstration
// ------------------------------------------------------------

int main()
{
    // Create the core application.
    // Note:
    //  - It starts with an empty plugin collection.
    CoreApplication core;
    
    // Install an action-only GreetingPlugin.
    // Note:
    //  - make_unique creates the concrete plugin and transfers ownership
    //    to CoreApplication through the ActionPlugin interface.
    core.install_plugin("greeting", std::make_unique<GreetingPlugin>());
    
    // Install an UppercasePlugin with its initial prefix configuration.
    // Note:
    //  - UppercasePlugin supports both ActionPlugin and ConfigurablePlugin.
    core.install_plugin("uppercase", std::make_unique<UppercasePlugin>("UPPER"));
    
    // Execute the GreetingPlugin through CoreApplication.
    // Note:
    //  - The core looks up the plugin by ID and calls execute() polymorphically.
    std::cout << core.run_plugin("greeting", "hello") << '\n';
    
    // Execute the UppercasePlugin with its initial configuration.
    std::cout << core.run_plugin("uppercase", "hello") << '\n';
    
    // Reconfigure the installed UppercasePlugin.
    // Note:
    //  - CoreApplication detects the ConfigurablePlugin capability at runtime
    //    and delegates the configuration change to the concrete plugin.
    core.configure_plugin("uppercase", "Upper");
    
    // Execute the same plugin again to demonstrate that the updated
    // configuration persists between executions.
    std::cout << core.run_plugin("uppercase", "hello") << '\n';
    
    // Attempt to configure GreetingPlugin.
    // Note:
    //  - GreetingPlugin implements ActionPlugin only, so the runtime
    //    capability check rejects the configuration request.
    try {
        core.configure_plugin("greeting", "Something");
    }
    catch (const std::exception& error) {
        std::cout << "Configuration error: " << error.what() << '\n';
    }
    
    // Remove GreetingPlugin from the application.
    // Note: 
    //  - Erasing the map entry destroys its unique_ptr and therefore
    //    automatically destroys the owned plugin object.
    core.remove_plugin("greeting");
    
    // Attempt to execute the removed plugin.
    // Note:
    //  - The lookup fails because the plugin ID is no longer installed.
    try {
        std::cout << core.run_plugin("greeting", "hello") << '\n';
    }
    catch (const std::exception& error) {
        std::cout << "Plugin error: " << error.what() << '\n';
    }
    return 0;
}
