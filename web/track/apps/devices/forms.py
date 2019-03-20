from django import forms

from .models import Device


class DeviceAddForm(forms.ModelForm):

    class Meta:
        model = Device
        fields = (
            'name',
        )

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        text_field_attrs = {
            'class': 'form-control',
            'autocomplete': 'off',
        }

        self.fields['name'].widget.attrs.update(text_field_attrs)

