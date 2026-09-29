#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>


// ------------------------------------------------------------
// Authentication interfaces
// ------------------------------------------------------------

// Small abstract interface dedicated only to password-based authentication.
// Notes:
//  - Keeping this capability separate supports Interface Segregation: classes that
//    only need password login are not forced to depend on OAuth or API-token methods.
//  - virtual + = 0 makes authenticate() pure virtual, so concrete password
//    authenticators must provide their own implementation.
//  - std::string_view is passed by value because it is a lightweight,
//    non-owning read-only view of the supplied credentials.
//  - Trailing const means authentication does not modify the authenticator object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class PasswordAuthenticator {
public:
    virtual ~PasswordAuthenticator() = default;

    virtual bool authenticate(
        std::string_view username,
        std::string_view password
    ) const = 0;
};


// Small abstract interface dedicated only to OAuth authentication.
// Notes:
//  - Keeping OAuth separate supports Interface Segregation: implementations that
//    handle OAuth are not forced to expose password or API-token operations
//  - virtual + = 0 makes authenticate() pure virtual, so concrete OAuth
//    authenticators must provide their own implementation.
//  - std::string_view is passed by value because it is a lightweight,
//    non-owning read-only view of the token.
//  - Trailing const means authentication does not modify the authenticator object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class OAuthAuthenticator {
public:
    virtual ~OAuthAuthenticator() = default;

    virtual bool authenticate(
        std::string_view token
    ) const = 0;
};


// Small abstract interface dedicated only to API-token authentication.
// Notes:
//  - Keeping API-token login separate supports Interface Segregation: implementations
//    are not forced to depend on password- or OAuth-specific operations.
//  - virtual + = 0 makes authenticate() pure virtual, so concrete API-token
//    authenticators must provide their own implementation.
//  - std::string_view is passed by value because it is a lightweight,
//    non-owning read-only view of the token.
//  - Trailing const means authentication does not modify the authenticator object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class ApiTokenAuthenticator {
public:
    virtual ~ApiTokenAuthenticator() = default;

    virtual bool authenticate(
        std::string_view token
    ) const = 0;
};


// ------------------------------------------------------------
// Password-specific responsibilities
// ------------------------------------------------------------

// Abstract interface responsible only for retrieving stored password hashes.
// Notes:
//  - Keeping storage separate from verification supports secure responsibility 
//    separation.
//  - std::optional expresses that a matching user/hash may not exist.
//  - The returned std::string_view is a lightweight, non-owning view of the stored 
//    hash, so the underlying storage must remain alive while the view is used.
//  - username is passed as string_view because it is only read during the lookup.
//  - virtual + = 0 makes the method pure virtual, requiring concrete stores to 
//    implement it.
//  - Trailing const means the lookup does not modify the PasswordStore object.
class PasswordStore {
public:
    virtual ~PasswordStore() = default;

    virtual std::optional<std::string_view> password_hash_for(
        std::string_view username
    ) const = 0;
};


// Abstract interface responsible only for password verification.
// Notes:
//  - Keeping verification separate from storage supports secure responsibility 
//    separation.
//  - Both inputs use std::string_view because they are read-only and only needed
//    during the verification call, avoiding unnecessary string copies.
//  - virtual + = 0 makes verify() pure virtual, so concrete verifiers must
//    provide their own verification implementation.
//  - Trailing const means verification does not modify the verifier object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class PasswordVerifier {
public:
    virtual ~PasswordVerifier() = default;

    virtual bool verify(
        std::string_view password,
        std::string_view stored_hash
    ) const = 0;
};


// Concrete in-memory implementation of the abstract base class PasswordStore.
class InMemoryPasswordStore : public PasswordStore {
private:
    // username_ and password_hash_ are owned by the object and use trailing
    // underscores to distinguish member variables from parameters.
    std::string username_;
    std::string password_hash_;

public:
    // Constructor.
    // Notes:
    //  - Take username and password_hash by value because the store keeps its 
    //    own copies. Move the local parameters into the member strings to avoid 
    //    an additional copy.
    InMemoryPasswordStore(
        std::string username,
        std::string password_hash
    )
        : username_(std::move(username)),
          password_hash_(std::move(password_hash))
    {
        // Validation ensures the store starts in a valid state.
        if (username_.empty() || password_hash_.empty()) {
            throw std::invalid_argument(
                "Username and password hash cannot be empty."
            );
        }
    }

    // Look up the stored password hash for a username.
    // Notes:
    //  - username is passed as std::string_view because it is only read during 
    //    the lookup.
    //  - std::optional expresses that a matching user may not exist.
    //  - If the username matches, return a non-owning string_view of password_hash_;
    //    otherwise return std::nullopt.
    //  - Trailing const means the lookup does not modify the store. 
    //  - override verifies that this method correctly implements 
    //    PasswordStore::password_hash_for().
    std::optional<std::string_view> password_hash_for(
        std::string_view username
    ) const override
    {
        if (username == username_) {
            return password_hash_;
        }

        return std::nullopt;
    }
};


// Demonstration-only concrete implementation of abstract base class PasswordVerifier.
class DemoPasswordVerifier : public PasswordVerifier {
public:
    // Password verification.
    // Notes:
    //  - verify() receives read-only string views to avoid unnecessary string copies.
    //  - A temporary std::string is created to simulate hashing the supplied password,
    //    then compared with the stored hash.
    //  - override verifies that the PasswordVerifier interface is implemented 
    //    correctly.
    //  - Trailing const means verification does not modify the verifier object.
    bool verify(
        std::string_view password,
        std::string_view stored_hash
    ) const override
    {
        const std::string simulated_hash =
            "demo_hash:" + std::string{password};

        return simulated_hash == stored_hash;
    }
};


// Concrete password-login coordinator implementing the abstract base class
// PasswordAuthenticator.
class PasswordLogin : public PasswordAuthenticator {
private:
    // PasswordStore and PasswordVerifier are injected by const reference because 
    // they are required, non-owning dependencies that should not be modified here.
    // This keeps credential storage and password verification separate from login 
    // logic.
    const PasswordStore& store_;
    const PasswordVerifier& verifier_;

public:
    // Constructor.
    // Notes:
    //  - Inject PasswordStore and PasswordVerifier as required non-owning 
    //    dependencies.
    //  - Pass by const reference to avoid copies and prevent modification.
    //  - The member references bind directly to the outside objects, so no ownership
    //    is transferred; therefore the supplied store and verifier must outlive 
    //    PasswordLogin.
    PasswordLogin(
        const PasswordStore& store,
        const PasswordVerifier& verifier
    )
        : store_(store),
          verifier_(verifier)
    {
    }

    // Implement password authentication by coordinating the injected dependencies.
    // Notes:
    //  - username and password use std::string_view because they are read-only and 
    //    only needed for the duration of the call.
    //  - First ask PasswordStore for the user's stored hash; std::optional lets us 
    //    detect that no matching user/hash exists without using a sentinel value.
    //  - If a hash is present, *stored_hash dereferences the optional and gives 
    //    access to the contained string_view, which is then passed to PasswordVerifier.
    //  - override verifies the PasswordAuthenticator interface is implemented 
    //    correctly.
    //  - Trailing const means authentication does not modify the PasswordLogin object.
    bool authenticate(
        std::string_view username,
        std::string_view password
    ) const override
    {
        const auto stored_hash =
            store_.password_hash_for(username);

        if (!stored_hash) {
            return false;
        }

        return verifier_.verify(
            password,
            *stored_hash
        );
    }
};


// ------------------------------------------------------------
// OAuth-specific responsibilities
// ------------------------------------------------------------

// Abstract interface responsible only for OAuth-token verification.
// Notes:
//  - Keeping OAuth verification separate supports secure responsibility separation.
//  - std::string_view is passed by value because the token is read-only and
//    only needed for the duration of the call.
//  - virtual + = 0 makes verify() pure virtual, so concrete OAuth verifiers
//    must provide their own verification logic.
//  - Trailing const means verification does not modify the verifier object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class OAuthTokenVerifier {
public:
    virtual ~OAuthTokenVerifier() = default;

    virtual bool verify(
        std::string_view token
    ) const = 0;
};


// Demonstration-only concrete implementation of abstract base class.
class DemoOAuthTokenVerifier : public OAuthTokenVerifier {
private:
    std::string valid_token_;

public:
    // Constructor.
    // Notes:
    //  - Owns valid_token_: the constructor takes the input by value and
    //    moves it into the member to avoid an additional copy.
    //  - explicit prevents unintended implicit conversion from std::string.
    explicit DemoOAuthTokenVerifier(std::string valid_token)
        : valid_token_(std::move(valid_token))
    {
        // Validation ensures the stored token is not empty.
        if (valid_token_.empty()) {
            throw std::invalid_argument(
                "OAuth token cannot be empty."
            );
        }
    }

    // Verification.
    // Notes:
    //  - verify() compares the supplied read-only string_view with the stored token;
    //  - override verifies the base interface is implemented correctly.
    //  - Trailing const means verification does not modify the verifier object.
    bool verify(
        std::string_view token
    ) const override
    {
        return token == valid_token_;
    }
};


// Concrete OAuth authentication login implementation for abstract base class.
class OAuthLogin : public OAuthAuthenticator {
private:
    // OAuthTokenVerifier is injected by const reference because it is a required,
    // non-owning dependency that should not be modified by OAuthLogin.
    // Notes: 
    //  - The reference member binds directly to the outside verifier, so no 
    //  copy ownership transfer occurs.
    //  - The verifier must outlive OAuthLogin.
    const OAuthTokenVerifier& verifier_;

public:
    // Constructor.
    // Notes:
    //  - Inject OAuthTokenVerifier as a required, non-owning dependency.
    //  - Pass it by const reference to avoid copying and prevent modification.
    //  - The reference member binds directly to the outside verifier object,
    //    so no ownership is transferred and the verifier must outlive OAuthLogin.
    explicit OAuthLogin(
        const OAuthTokenVerifier& verifier
    )
        : verifier_(verifier)
    {
    }

    // Authentification.
    // Notes:
    //  - authenticate() accepts the token as std::string_view because it is read-only
    //    and only needed during the call, then delegates verification to the verifier.
    //  - override verifies that OAuthAuthenticator::authenticate() is implemented 
    //    correctly.
    //  - Trailing const means authentication does not modify the OAuthLogin object.
    bool authenticate(
        std::string_view token
    ) const override
    {
        return verifier_.verify(token);
    }
};


// ------------------------------------------------------------
// API-token-specific responsibilities
// ------------------------------------------------------------

// Abstract interface responsible only for API-token verification.
// Notes:
//  - Keeping API-token verification separate supports secure responsibility separation.
//  - std::string_view is passed by value because the token is read-only and only
//    needed for the duration of the call.
//  - virtual + = 0 makes verify() pure virtual, so concrete API-token verifiers
//    must provide their own verification logic.
//  - Trailing const means verification does not modify the verifier object.
//  - The virtual destructor ensures safe destruction through a base-class pointer.
class ApiTokenVerifier {
public:
    virtual ~ApiTokenVerifier() = default;

    virtual bool verify(
        std::string_view token
    ) const = 0;
};


// Demonstration-only concrete implementation of abstract base class ApiTokenVerifier.
/
// verify() compares the supplied read-only string_view with the stored token.
// override verifies that ApiTokenVerifier::verify() is implemented correctly.
// trailing const means verification does not modify the verifier object.
// Real API-token verification should use an appropriate credential/token service
// or secure comparison mechanism rather than a raw string comparison.
class DemoApiTokenVerifier : public ApiTokenVerifier {
private:
    std::string valid_token_;

public:
    // Constructor.
    // Notes:
    //  - valid_token_: the constructor takes the input by value and moves it into the 
    //    member to avoid an additional copy.
    //  - explicit prevents unintended implicit conversion from std::string.
    //  - Validation ensures the stored token is not empty.
    explicit DemoApiTokenVerifier(std::string valid_token)
        : valid_token_(std::move(valid_token))
    {
        if (valid_token_.empty()) {
            throw std::invalid_argument(
                "API token cannot be empty."
            );
        }
    }

    // verification.
    // Notes:
    //  - Compare the supplied API token with the verifier's stored valid token.
    //  - std::string_view avoids copying the input because it is only read here.
    //  - Trailing const means verification does not modify the verifier object.
    //  - override verifies that ApiTokenVerifier::verify() is implemented correctly.
    bool verify(
        std::string_view token
    ) const override
    {
        return token == valid_token_;
    }
};


// Concrete API-token authentication implementation.
class ApiTokenLogin : public ApiTokenAuthenticator {
private:
    const ApiTokenVerifier& verifier_;

public:
    explicit ApiTokenLogin(
        const ApiTokenVerifier& verifier
    )
        : verifier_(verifier)
    {
    }

    bool authenticate(
        std::string_view token
    ) const override
    {
        return verifier_.verify(token);
    }
};


// ------------------------------------------------------------
// Authentication service
// ------------------------------------------------------------

// High-level service responsible only for coordinating login.
// Concrete authentication mechanisms are injected from outside.
class AuthenticationService {
private:
    const PasswordAuthenticator& password_authenticator_;
    const OAuthAuthenticator& oauth_authenticator_;
    const ApiTokenAuthenticator& api_token_authenticator_;

public:
    AuthenticationService(
        const PasswordAuthenticator& password_authenticator,
        const OAuthAuthenticator& oauth_authenticator,
        const ApiTokenAuthenticator& api_token_authenticator
    )
        : password_authenticator_(password_authenticator),
          oauth_authenticator_(oauth_authenticator),
          api_token_authenticator_(api_token_authenticator)
    {
    }

    bool login_with_password(
        std::string_view username,
        std::string_view password
    ) const
    {
        return password_authenticator_.authenticate(
            username,
            password
        );
    }

    bool login_with_oauth(
        std::string_view token
    ) const
    {
        return oauth_authenticator_.authenticate(token);
    }

    bool login_with_api_token(
        std::string_view token
    ) const
    {
        return api_token_authenticator_.authenticate(token);
    }
};


int main()
{
    try {
        // Password infrastructure.
        InMemoryPasswordStore password_store{
            "alice",
            "demo_hash:secret123"
        };

        DemoPasswordVerifier password_verifier;

        PasswordLogin password_login{
            password_store,
            password_verifier
        };

        // OAuth infrastructure.
        DemoOAuthTokenVerifier oauth_verifier{
            "oauth-valid-token"
        };

        OAuthLogin oauth_login{
            oauth_verifier
        };

        // API-token infrastructure.
        DemoApiTokenVerifier api_token_verifier{
            "api-valid-token"
        };

        ApiTokenLogin api_token_login{
            api_token_verifier
        };

        // Dependency Injection:
        // AuthenticationService receives all authentication mechanisms
        // from outside instead of constructing concrete implementations.
        AuthenticationService authentication{
            password_login,
            oauth_login,
            api_token_login
        };

        std::cout << std::boolalpha;

        std::cout
            << "Password login: "
            << authentication.login_with_password(
                   "alice",
                   "secret123"
               )
            << '\n';

        std::cout
            << "OAuth login: "
            << authentication.login_with_oauth(
                   "oauth-valid-token"
               )
            << '\n';

        std::cout
            << "API token login: "
            << authentication.login_with_api_token(
                   "api-valid-token"
               )
            << '\n';

        std::cout
            << "Invalid password: "
            << authentication.login_with_password(
                   "alice",
                   "wrong-password"
               )
            << '\n';
    }
    catch (const std::exception& error) {
        std::cerr
            << "Error: "
            << error.what()
            << '\n';
    }

    return 0;
}
