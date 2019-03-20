from django.conf.urls import url
from django.contrib.auth.decorators import login_required

from . import views


app_name = 'profile'
urlpatterns = [
    url(r'$', login_required(views.ProfileView.as_view()), name='home'),
]

