from django.contrib.auth import get_user_model
from django.db import models
from django.db.models import Max
from django.db.models.functions import Coalesce

from .functions import calculate_hash
from . import const


class Device(models.Model):
    user = models.ForeignKey(get_user_model(), on_delete=models.CASCADE, related_name='device_set')
    sequence_id = models.IntegerField()
    name = models.CharField(max_length=30)
    token = models.CharField(max_length=const.DEVICE_TOKEN_LEN)
    salt = models.CharField(max_length=const.DEVICE_SALT_LEN)
    api_key_hash = models.CharField(max_length=64)

    class Meta:
        indexes = [
            models.Index(fields=['token']),
        ]
        unique_together = (
            ('user', 'sequence_id'),
            ('user', 'name'),
        )
        ordering = [
            'sequence_id',
        ]

    def get_api_key_display(self):
        return '{}{}'.format(self.token, '*' * (const.DEVICE_API_KEY_LEN - const.DEVICE_TOKEN_LEN))

    def is_matching_api_key(self, api_key):
        return calculate_hash(api_key, self.salt) == self.api_key_hash

    def save(self, *args, **kwargs):
        if self.pk is None:
            last_seq_id = Device.objects \
                .filter(user=self.user) \
                .aggregate(ret=Coalesce(Max('sequence_id'), 0))['ret']

            self.sequence_id = last_seq_id + 1

        super().save(*args, **kwargs)

    def __str__(self):
        return 'Device {} {}: {}'.format(
            self.user,
            self.sequence_id,
            self.name,
        )

