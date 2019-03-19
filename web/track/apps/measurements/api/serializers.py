from decimal import Decimal

from django.utils import timezone
from rest_framework import serializers

from measurements.models import GpsMeasurement


class GpsMeasurementSerializer(serializers.Serializer):
    lat = serializers.CharField()
    long = serializers.CharField()
    date = serializers.CharField()

    def _validate_position(self, value):
        return Decimal(value)

    def validate_lat(self, value):
        return self._validate_position(value)

    def validate_long(self, value):
        return self._validate_position(value)

    def validate_date(self, value):
        return timezone.now()

    def create(self, validated_data):
        return GpsMeasurement.objects.create(
            device=self.context['device'],
            latitude=validated_data['lat'],
            longitude=validated_data['long'],
            date_collected=validated_data['date'],
        )

