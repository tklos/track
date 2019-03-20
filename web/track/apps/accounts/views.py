import django.contrib.auth.views as auth_views
from django.contrib import messages
from django.urls import reverse_lazy
from django.views.generic import CreateView

from .forms import LoginForm, RegisterForm


class LoginView(auth_views.LoginView):
    form_class = LoginForm
    template_name = 'accounts/login.html'


class LogoutView(auth_views.LogoutView):
    template_name = 'accounts/logout.html'


class RegisterView(CreateView):
    form_class = RegisterForm
    template_name = 'accounts/register.html'
    success_url = reverse_lazy('account:login')

    def form_valid(self, form):
        ret = super().form_valid(form)
        messages.success(self.request, 'User {} registered. You can log in now'.format(self.object.username))
        return ret

