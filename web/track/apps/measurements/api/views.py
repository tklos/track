from rest_framework import status
from rest_framework.response import Response
from rest_framework.views import APIView

from devices.authentication import ApiKeyAuthentication
from .serializers import GpsMeasurementSerializer


class MeasurementView(APIView):
    authentication_classes = (
        ApiKeyAuthentication,
    )

    def post(self, request):
        context = {
            'device': request.user,
        }

        serializer = GpsMeasurementSerializer(data=request.data, context=context)
        if not serializer.is_valid():
            data = {
                'status': 'error',
                'errors': serializer.errors,
            }
            return Response(data=data, status=status.HTTP_400_BAD_REQUEST)

        serializer.save()

        data = {
            'status': 'ok',
        }
        return Response(data=data, status=status.HTTP_201_CREATED)

