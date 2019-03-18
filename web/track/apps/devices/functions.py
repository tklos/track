import hashlib


def calculate_hash(api_key, salt):
    to_hash = api_key + salt
    return hashlib.sha256(to_hash.encode()).hexdigest()

