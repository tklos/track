from django.contrib import admin
from django.urls import path, include
from django.views.generic.base import RedirectView


urlpatterns = [
    path('', RedirectView.as_view(pattern_name='profile:home'), name='index'),

    path('account/', include('accounts.urls')),
    path('profile/', include('profiles.urls')),
    path('devices/', include('devices.urls')),

    path('django-admin/', admin.site.urls),

    path('api/measurements/', include('measurements.api.urls')),
]

