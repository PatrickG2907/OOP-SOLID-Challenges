from abc import ABC, abstractmethod
from typing import List

# ------------------------------
# SRP: Book class has one responsibility
# ------------------------------
class Book:
    """Represents a single book with immutable title and author."""
    def __init__(self, title: str, author: str, available: bool = True):
        self._title = title
        self._author = author
        self.available = available  # mutable, e.g., for borrowing

    @property
    def title(self) -> str:
        return self._title

    @property
    def author(self) -> str:
        return self._author

# ------------------------------
# OCP: Search strategies can be extended without modifying Library
# ------------------------------
class SearchStrategy(ABC):
    """Abstract search strategy interface."""
    @abstractmethod
    def search(self, books: List[Book], query: str) -> List[Book]:
        pass

class TitleSearchStrategy(SearchStrategy):
    def search(self, books: List[Book], query: str) -> List[Book]:
        return [book for book in books if query.lower() in book.title.lower()]

class AuthorSearchStrategy(SearchStrategy):
    def search(self, books: List[Book], query: str) -> List[Book]:
        return [book for book in books if query.lower() in book.author.lower()]

# ------------------------------
# ISP & DIP: Repository interface
# ------------------------------
class BookRepositoryInterface(ABC):
    @abstractmethod
    def add_book(self, book: Book):
        pass

    @abstractmethod
    def list_books(self) -> List[Book]:
        pass

# ------------------------------
# LSP & DIP: Concrete repository
# ------------------------------
class BookRepository(BookRepositoryInterface):
    def __init__(self):
        self._books: List[Book] = []

    def add_book(self, book: Book):
        self._books.append(book)

    def list_books(self) -> List[Book]:
        return self._books

# ------------------------------
# Library: high-level module
# ------------------------------
class Library:
    """Manages books using a repository and search strategies."""
    def __init__(self, repository: BookRepositoryInterface):
        self.repository = repository

    def add_book(self, book: Book):
        self.repository.add_book(book)

    def get_available_books(self) -> List[Book]:
        """Return a list of available books."""
        return [book for book in self.repository.list_books() if book.available]

    def search_books(self, query: str, strategy: SearchStrategy) -> List[Book]:
        """Return search results as a list of books."""
        return strategy.search(self.repository.list_books(), query)

# ------------------------------
# Application layer: Handles presentation
# ------------------------------
if __name__ == "__main__":
    repo = BookRepository()
    library = Library(repo)

    # Add books
    library.add_book(Book("The Great Gatsby", "F. Scott Fitzgerald"))
    library.add_book(Book("To Kill a Mockingbird", "Harper Lee", available=False))
    library.add_book(Book("Great Expectations", "Charles Dickens"))

    # Display available books
    print("Available Books:")
    for book in library.get_available_books():
        print(f"{book.title} by {book.author}")

    # Search by title
    print("\nSearch by title 'Great':")
    title_strategy = TitleSearchStrategy()
    for book in library.search_books("Great", title_strategy):
        print(f"{book.title} by {book.author}")

    # Search by author
    print("\nSearch by author 'Harper':")
    author_strategy = AuthorSearchStrategy()
    for book in library.search_books("Harper", author_strategy):
        print(f"{book.title} by {book.author}")
