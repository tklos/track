from django.contrib.auth.models import AbstractUser
from django.db.models.functions import Lower


class User(AbstractUser):

    class Meta:
        ordering = [
            Lower('username'),
        ]

