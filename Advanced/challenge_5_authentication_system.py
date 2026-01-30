from abc import ABC, abstractmethod
from typing import Dict, List

# ----------------------------
# Interfaces / Abstractions
# ----------------------------

# Base authenticator interface (OOP: abstraction, SOLID: DIP + LSP)
class Authenticator(ABC):
    @abstractmethod
    def authenticate(self, credentials: Dict) -> bool:
        """Authenticate using provided credentials."""
        pass

# Specialized interface for OAuth (SOLID: ISP)
class OAuthAuthenticator(Authenticator):
    @abstractmethod
    def get_redirect_url(self) -> str:
        """Return the URL for OAuth authorization."""
        pass

# Repository abstractions (DIP)
class UserRepository(ABC):
    @abstractmethod
    def find_by_username(self, username: str):
        pass

class TokenRepository(ABC):
    @abstractmethod
    def is_valid(self, token: str) -> bool:
        pass

# ---------------------------
# Concrete Implementations
# ---------------------------

# Simple User class (OOP: encapsulation, SRP)
class User:
    def __init__(self, username: str, password: str):
        self.username = username
        self._password = password  # private attribute

    def check_password(self, password: str) -> bool:
        return self._password == password

# In-memory repositories (SRP + DIP)
class InMemoryUserRepository(UserRepository):
    def __init__(self):
        self.users = {"alice": "1234", "bob": "5678"}

    def find_by_username(self, username: str):
        if username in self.users:
            return User(username, self.users[username])
        return None

class InMemoryTokenRepository(TokenRepository):
    def __init__(self):
        self.valid_tokens = {"abc123", "xyz789"}

    def is_valid(self, token: str) -> bool:
        return token in self.valid_tokens

# Fake OAuth client (SRP)
class FakeOAuthClient:
    def get_authorize_url(self) -> str:
        return "https://oauth.example.com/authorize"

    def verify_token(self, token: str) -> bool:
        return token == "valid_oauth_token"

# ------------------------------------
# Authenticators: Low-Level Modules
# ------------------------------------

# Password-based authenticator (SRP + DIP)
class PasswordAuthenticator(Authenticator):
    def __init__(self, user_repository: UserRepository):
        self.user_repository = user_repository

    def authenticate(self, credentials: Dict) -> bool:
        username = credentials.get("username")
        password = credentials.get("password")
        user = self.user_repository.find_by_username(username)
        return user is not None and user.check_password(password)

# OAuth authenticator (SRP + ISP + DIP)
class OAuthGoogleAuthenticator(OAuthAuthenticator):
    def __init__(self, oauth_client: FakeOAuthClient):
        self.oauth_client = oauth_client

    def get_redirect_url(self) -> str:
        return self.oauth_client.get_authorize_url()

    def authenticate(self, credentials: Dict) -> bool:
        token = credentials.get("token")
        return self.oauth_client.verify_token(token)

# API token authenticator (SRP + DIP)
class ApiTokenAuthenticator(Authenticator):
    def __init__(self, token_repository: TokenRepository):
        self.token_repository = token_repository

    def authenticate(self, credentials: Dict) -> bool:
        token = credentials.get("api_token")
        return self.token_repository.is_valid(token)

# ----------------------------
# Authentication Service
# ----------------------------

# High-level module (DIP + OCP: open for extension, closed for modification)
class AuthenticationService:
    def __init__(self, authenticators: Dict[str, Authenticator]):
        """
        authenticators: dict mapping auth_type to Authenticator instance
        """
        self.authenticators = authenticators

    def login(self, auth_type: str, credentials: Dict) -> bool:
        """
        Login using the requested authentication type.
        """
        auth = self.authenticators.get(auth_type)
        if auth:
            return auth.authenticate(credentials)
        return False

# ----------------------------
# Setup / Usage
# ----------------------------

# Create repositories / clients
user_repo = InMemoryUserRepository()
token_repo = InMemoryTokenRepository()
oauth_client = FakeOAuthClient()

# Create authenticators
authenticators = {
    "password": PasswordAuthenticator(user_repo),
    "oauth": OAuthGoogleAuthenticator(oauth_client),
    "api_token": ApiTokenAuthenticator(token_repo)
}

# Create the authentication service
auth_service = AuthenticationService(authenticators)

# ----------------------------
# Test Cases
# ----------------------------

print("Password login (alice):", auth_service.login("password", {"username": "alice", "password": "1234"}))  # True
print("OAuth login:", auth_service.login("oauth", {"token": "valid_oauth_token"}))  # True
print("API token login:", auth_service.login("api_token", {"api_token": "abc123"}))  # True
print("Wrong password login:", auth_service.login("password", {"username": "alice", "password": "wrong"}))  # False
print("Invalid token login:", auth_service.login("api_token", {"api_token": "bad"}))  # False
