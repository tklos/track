import string
import random

from django.contrib import messages
from django.shortcuts import get_object_or_404
from django.urls import reverse_lazy
from django.utils.safestring import mark_safe
from django.views.generic import TemplateView, CreateView, DetailView

from .models import Device
from .forms import DeviceAddForm
from .functions import calculate_hash
from . import const


class DeviceAddView(CreateView):
    form_class = DeviceAddForm
    template_name = 'profiles/home.html'
    success_url = reverse_lazy('profile:home')

    def form_valid(self, form):
        user = form.instance.user = self.request.user
        name = form.cleaned_data['name']

        # Check if name is correct
        if Device.objects.filter(user=user, name=name).first() is not None:
            form.add_error('name', 'Device with this name already exists')
            return self.form_invalid(form)

        # Create API key
        api_key = ''.join(random.sample(string.ascii_letters + string.digits, k=const.DEVICE_API_KEY_LEN))
        token = api_key[:const.DEVICE_TOKEN_LEN]
        salt = ''.join(random.sample(string.ascii_letters + string.digits, k=const.DEVICE_SALT_LEN))
        api_key_hash = calculate_hash(api_key, salt)

        form.instance.token = token
        form.instance.salt = salt
        form.instance.api_key_hash = api_key_hash

        ret = super().form_valid(form)

        messages.success(self.request, mark_safe('Device {} added.<br/>API key: <b>{}</b><br/>Please save it as it is shown only once'.format(name, api_key)))

        return ret

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)

        context['device_add_form'] = context.pop('form')

        return context

    def form_invalid(self, form):
        messages.error(self.request, 'Adding device failed')
        return super().form_invalid(form)


class DeviceView(DetailView):
    template_name = 'devices/device.html'

    def get_object(self, queryset=None):
        return get_object_or_404(Device.objects, user=self.request.user, sequence_id=self.kwargs['d_sid'])

