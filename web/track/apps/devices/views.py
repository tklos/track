import csv
import string
import random

from django.conf import settings
from django.contrib import messages
from django.core.paginator import Paginator
from django.http import HttpResponse, JsonResponse, HttpResponseRedirect
from django.shortcuts import get_object_or_404
from django.urls import reverse_lazy, reverse
from django.utils.safestring import mark_safe
from django.views.generic import TemplateView, CreateView, DetailView, DeleteView
from django.views.generic.detail import BaseDetailView

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


class DeviceGpsMeasurementsMixin:
    PAGINATE_BY = 20

    def get_gps_measurements_context(self, device, page):
        gps_measurements = device.gps_measurement_set.all()
        num_gps_measurements = gps_measurements.count()

        gps_measurements_paginator = Paginator(gps_measurements, self.PAGINATE_BY)
        this_gps_measurements = gps_measurements_paginator.get_page(page)

        return {
            'gps_measurements': reversed(gps_measurements),

            'table_gps_measurements': this_gps_measurements,
            'table_gps_measurements_ids': {gm.id: num_gps_measurements-this_gps_measurements.start_index()+1-ind for ind, gm in enumerate(this_gps_measurements)},
            'extra': {'d_sid': device.sequence_id},
        }


class DeviceView(DeviceGpsMeasurementsMixin, DetailView):
    template_name = 'devices/device.html'

    def get_object(self, queryset=None):
        return get_object_or_404(Device.objects, user=self.request.user, sequence_id=self.kwargs['d_sid'])

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        m_context = self.get_gps_measurements_context(self.object, 1)
        context.update(m_context)

        context['maps_api_key'] = settings.MAPS_API_KEY

        return context


class DeviceDeleteView(DeleteView):
    success_url = reverse_lazy('profile:home')

    def get_object(self, queryset=None):
        return get_object_or_404(Device.objects, user=self.request.user, sequence_id=self.kwargs['d_sid'])

    def get(self, request, *args, **kwargs):
        raise NotImplementedError()

    def post(self, request, *args, **kwargs):
        ret = super().post(request, *args, **kwargs)

        messages.success(self.request, 'Device {} deleted'.format(self.object.name))

        return ret


class DeviceDataDeleteView(DeleteView):

    def get_object(self, queryset=None):
        return get_object_or_404(Device.objects, user=self.request.user, sequence_id=self.kwargs['d_sid'])

    def get_success_url(self):
        return reverse('devices:device', kwargs=self.kwargs)

    def get(self, request, *args, **kwargs):
        raise NotImplementedError()

    def delete(self, request, *args, **kwargs):
        object = self.get_object()
        success_url = self.get_success_url()

        object.gps_measurement_set.all().delete()

        messages.success(self.request, 'Data deleted')

        return HttpResponseRedirect(success_url)


class DeviceDownloadCsvView(BaseDetailView):

    def get_object(self, queryset=None):
        return get_object_or_404(Device.objects, user=self.request.user, sequence_id=self.kwargs['d_sid'])

    def render_to_response(self, context, **response_kwargs):
        response = HttpResponse(content_type='text/csv')
        response['Content-Disposition'] = 'attachment; filename="gps-measurements.csv"'

        writer = csv.writer(response)
        writer.writerow(['Date collected', 'Latitude', 'Longitude'])

        for m in self.object.gps_measurement_set.all().order_by('date_collected'):
            writer.writerow([m.date_collected, m.latitude, m.longitude])

        return response


class PaginationGpsMeasurementsView(DeviceGpsMeasurementsMixin, TemplateView):
    template_name = 'devices/device_gps_measurements.html'

    def dispatch(self, request, *args, **kwargs):
        assert request.is_ajax(), 'Ajax request expected'
        return super().dispatch(request, *args, **kwargs)

    def get_context_data(self, d_sid, page, **kwargs):
        device = Device.objects.get(user=self.request.user, sequence_id=d_sid)

        context = super().get_context_data(**kwargs)
        m_context = self.get_gps_measurements_context(device, page)
        context.update(m_context)

        return context

    def get(self, request, *args, **kwargs):
        response = super().get(request, *args, **kwargs)

        data = {
            'html': response.rendered_content,
        }
        return JsonResponse(data)

