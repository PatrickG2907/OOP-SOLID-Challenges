#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Book {
// Trailing underscore marks member variables.
// In-class initialization makes new books available by default.
private:
    std::string title_;
    bool available_{true};

public:
    // Constructor:
    // - Uses a member initializer list to initialize title_ directly.
    // - Uses std::move to avoid an unnecessary string copy.
    // - Validates that the title is not empty.
    // - explicit prevents unintended implicit conversion from std::string to Book,
    //   so a Book must be created intentionally, e.g. Book{"Clean Code"}.
    explicit Book(std::string title)
        : title_(std::move(title))
    {
        if (title_.empty()) {
            throw std::invalid_argument(
                "Book title cannot be empty."
            );
        }
    }

    // Return by const reference to avoid copying the string.
    const std::string& title() const noexcept
    {
        return title_;
    }

    // Return bool by value because primitive types are cheap to copy.
    bool is_available() const noexcept
    {
        return available_;
    }

    void borrow()
    {
        if (!available_) {
            throw std::runtime_error(
                "Book is already borrowed."
            );
        }

        available_ = false;
    }

    void return_book() noexcept
    {
        available_ = true;
    }
};

class Library {
private:
    // Use std::vector to store multiple Book objects.
    // Explicit std:: qualification avoids importing the entire namespace,
    // and the trailing underscore marks books_ as a member variable.
    std::vector<Book> books_;

public:
    // Take Book by value because the Library needs to keep its own Book object.
    // Storing only a reference could become unsafe if the original Book is destroyed.
    void add_book(Book book)
    {
        books_.push_back(std::move(book));
    }

    // Search by title without copying the search string.
    // std::find_if returns an iterator to the first matching Book.
    // The lambda captures title by reference and inspects each Book by const reference.
    // If no match is found, return nullptr.
    // Otherwise, *it gives the Book object and &(*it) returns its memory address,
    // which matches the const Book* return type.
    const Book* find_by_title(const std::string& title) const
    {
        const auto it = std::find_if(
            books_.begin(),
            books_.end(),
            [&title](const Book& book) {
                return book.title() == title;
            }
        );

        if (it == books_.end()) {
            return nullptr;
        }

        return &(*it);
    }

    // Expose the collection as read-only and avoid copying the vector.
    const std::vector<Book>& books() const noexcept
    {
        return books_;
    }
};


// Presentation is kept separate from the Library class.
// Pass Library by const reference to avoid copying the entire collection
// while preventing the function from modifying the Library.
void print_available_books(const Library& library)
{
    std::cout << "Available books:\n";

    // Range-based for loop: inspect each Book by const reference
    // to avoid copying while preventing modification.
    for (const Book& book : library.books()) {
        if (book.is_available()) {
            std::cout << "- " << book.title() << '\n';
        }
    }
}


int main()
{
    try {
        Library library;

        library.add_book(Book{"Clean Code"});
        library.add_book(Book{"The Pragmatic Programmer"});
        library.add_book(Book{"Design Patterns"});

        print_available_books(library);

        const Book* book =
            library.find_by_title("The Pragmatic Programmer");

        // Access a member through the Book pointer.
        // book->title() is shorthand for (*book).title().
        if (book != nullptr) {
            std::cout << "\nFound: "
                      << book->title()
                      << '\n';
        }
        else {
            std::cout << "\nBook not found.\n";
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what()
                  << '\n';
    }

    return 0;
}
