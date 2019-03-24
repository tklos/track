from datetime import datetime, timezone
from decimal import Decimal

from rest_framework import serializers

from measurements.models import GpsMeasurement


class GpsMeasurementSerializer(serializers.Serializer):
    lat = serializers.CharField()
    long = serializers.CharField()
    date = serializers.CharField()

    def validate_lat(self, value):
        value = Decimal(value)
        if abs(value) >= 180.:
            raise serializers.ValidationError('Latitude is incorrect: {}'.format(value));
        return value

    def validate_long(self, value):
        value = Decimal(value)
        if abs(value) >= 90.:
            raise serializers.ValidationError('Longitude is incorrect: {}'.format(value));
        return value

    def validate_date(self, value):
        return datetime.strptime(value, '%Y-%m-%dT%H:%M:%SZ').replace(tzinfo=timezone.utc)

    def create(self, validated_data):
        return GpsMeasurement.objects.create(
            device=self.context['device'],
            latitude=validated_data['lat'],
            longitude=validated_data['long'],
            date_collected=validated_data['date'],
        )

