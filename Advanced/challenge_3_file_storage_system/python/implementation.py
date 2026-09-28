from abc import ABC, abstractmethod
import os
import re

# -------------------------------
# OOP: Abstraction / Interface
# SOLID: DIP, ISP
# -------------------------------
class Storage(ABC):
    @abstractmethod
    def save_file(self, filename: str, data: bytes) -> None:
        pass

    @abstractmethod
    def read_file(self, filename: str) -> bytes:
        pass

# -------------------------------
# SRP: Validates filenames
# -------------------------------
class FilenameValidator:
    FILENAME_PATTERN = r"^[\w\-. ]+$"

    @staticmethod
    def validate(filename: str) -> bool:
        if not filename or len(filename) > 255:
            return False
        return bool(re.match(FilenameValidator.FILENAME_PATTERN, filename))

# -------------------------------
# SRP: Validates directories
# -------------------------------
class DirectoryValidator:
    @staticmethod
    def validate(path: str) -> bool:
        return os.path.isdir(path) and os.access(path, os.W_OK)

# -------------------------------
# OOP & SRP: Local storage only
# -------------------------------
class LocalStorage(Storage):
    def __init__(self, base_path: str):
        if not DirectoryValidator.validate(base_path):
            raise ValueError(f"Invalid base path: '{base_path}'")
        self.base_path = base_path

    def save_file(self, filename: str, data: bytes) -> None:
        with open(os.path.join(self.base_path, filename), "wb") as f:
            f.write(data)
        print(f"[LocalStorage] File '{filename}' saved locally.")

    def read_file(self, filename: str) -> bytes:
        with open(os.path.join(self.base_path, filename), "rb") as f:
            return f.read()

# -------------------------------
# OOP & SRP: Cloud storage only
# -------------------------------
class CloudStorage(Storage):
    def __init__(self, cloud_client, bucket_name: str):
        self.cloud_client = cloud_client
        self.bucket_name = bucket_name

    def save_file(self, filename: str, data: bytes) -> None:
        self.cloud_client.upload(bucket=self.bucket_name, key=filename, data=data)
        print(f"[CloudStorage] File '{filename}' saved to cloud bucket '{self.bucket_name}'.")

    def read_file(self, filename: str) -> bytes:
        return self.cloud_client.download(bucket=self.bucket_name, key=filename)

# -------------------------------
# OOP: FileManager with Dependency Injection
# -------------------------------
class FileManager:
    def __init__(self, storage: Storage):
        self.storage = storage

    def save(self, filename: str, data: bytes):
        if not FilenameValidator.validate(filename):
            raise ValueError(f"Invalid filename: '{filename}'")
        self.storage.save_file(filename, data)

    def read(self, filename: str) -> bytes:
        if not FilenameValidator.validate(filename):
            raise ValueError(f"Invalid filename: '{filename}'")
        return self.storage.read_file(filename)

# -------------------------------
# Dummy Cloud Client for demonstration
# -------------------------------
class DummyCloudClient:
    def __init__(self):
        self.store = {}
    def upload(self, bucket, key, data):
        self.store[(bucket, key)] = data
    def download(self, bucket, key):
        return self.store.get((bucket, key), b"")

# -------------------------------
# Usage Example
# -------------------------------
if __name__ == "__main__":
    # Local storage with valid path
    try:
        local_storage = LocalStorage("/tmp")  # Must exist and be writable
        local_manager = FileManager(local_storage)
        local_manager.save("local_file.txt", b"Hello Local!")
        print(local_manager.read("local_file.txt"))
    except ValueError as e:
        print(e)

    # Cloud storage
    cloud_storage = CloudStorage(DummyCloudClient(), "my-bucket")
    cloud_manager = FileManager(cloud_storage)
    cloud_manager.save("cloud_file.txt", b"Hello Cloud!")
    print(cloud_manager.read("cloud_file.txt"))

    # Invalid filename
    try:
        local_manager.save("../hack.txt", b"Hack attempt")
    except ValueError as e:
        print(e)

    # Invalid base path
    try:
        LocalStorage("/invalid/path")
    except ValueError as e:
        print(e)
