from django.views.generic import TemplateView

from devices.forms import DeviceAddForm


class ProfileView(TemplateView):
    template_name = 'profiles/home.html'

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context.update({
            'device_add_form': DeviceAddForm(),
        })

        return context

