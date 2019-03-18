from django.views.generic import TemplateView


class ProfileView(TemplateView):
    template_name = 'profiles/home.html'

