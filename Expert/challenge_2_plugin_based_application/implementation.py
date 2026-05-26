from abc import ABC, abstractmethod
import importlib
import pkgutil

# =========================
# INTERFACE (OOP + SOLID)
# =========================
# - Interface Segregation Principle
# - Dependency Inversion Principle
# Core depends on this abstraction, NOT on concrete plugins
class Plugin(ABC):

    @abstractmethod
    def run(self, data: str) -> str:
        pass

# =========================
# CORE APPLICATION
# =========================
# - Single Responsibility Principle (only orchestrates plugins)
# - Open/Closed Principle (new plugins without modification)
class Application:

    def __init__(self):
        self._plugins: list[Plugin] = []

    # Dependency Inversion:
    # Core receives abstractions, not concrete implementations
    def register_plugin(self, plugin: Plugin) -> None:
        self._plugins.append(plugin)

    def run(self, data: str) -> None:
        for plugin in self._plugins:
            print(plugin.run(data))

# =========================
# PLUGIN LOADER (DYNAMIC)
# =========================
# - Dynamic extensibility
# - Core still does NOT know plugin internals
def load_plugins(app: Application, package_name: str) -> None:
    package = importlib.import_module(package_name)

    for _, module_name, _ in pkgutil.iter_modules(package.__path__):
        module = importlib.import_module(f"{package_name}.{module_name}")

        # Find classes that implement Plugin interface
        for attribute in vars(module).values():
            if (
                isinstance(attribute, type)
                and issubclass(attribute, Plugin)
                and attribute is not Plugin
            ):
                app.register_plugin(attribute())

# =========================
# EXAMPLE PLUGINS
# =========================
# Imagine these live in: plugins/uppercase.py
class UppercasePlugin(Plugin):
    # Liskov Substitution Principle:
    # Can replace Plugin without breaking behavior
    def run(self, data: str) -> str:
        return data.upper()

# Imagine these live in: plugins/reverse.py
class ReversePlugin(Plugin):
    def run(self, data: str) -> str:
        return data[::-1]

# =========================
# BOOTSTRAP
# =========================
if __name__ == "__main__":
    app = Application()

    # Plugins registered dynamically
    # app knows NOTHING about their concrete classes
    app.register_plugin(UppercasePlugin())
    app.register_plugin(ReversePlugin())

    app.run("Hello SOLID World")
