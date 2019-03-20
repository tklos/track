from django.conf.urls import url
from django.contrib.auth.decorators import login_required

from . import views


app_name = 'account'
urlpatterns = [
    url('^login/$', views.LoginView.as_view(), name='login'),
    url('^logout/$', login_required(views.LogoutView.as_view()), name='logout'),
    url('^register/$', views.RegisterView.as_view(), name='register'),
]

