from django.urls import path

from . import views


urlpatterns = [
    path('', views.MeasurementView.as_view()),
]

