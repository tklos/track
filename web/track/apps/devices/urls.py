from django.contrib.auth.decorators import login_required
from django.urls import path

from . import views


app_name = 'devices'
urlpatterns = [
    path('add/', login_required(views.DeviceAddView.as_view()), name='add'),
    path('<int:d_sid>/', login_required(views.DeviceView.as_view()), name='device'),
    path('<int:d_sid>/download-csv/', login_required(views.DeviceDownloadCsvView.as_view()), name='download-csv'),
]
