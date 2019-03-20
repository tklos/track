from django import forms
import django.contrib.auth.forms as auth_forms
from django.contrib.auth import password_validation, get_user_model
from django.core.exceptions import ValidationError
from django.utils.safestring import mark_safe

from users.validators import UsernameValidator


class LoginForm(auth_forms.AuthenticationForm):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        text_field_attrs = {
            'class': 'form-control',
        }
        self.fields['username'].widget.attrs.update(text_field_attrs)
        self.fields['password'].widget.attrs.update(text_field_attrs)

        self.error_messages['invalid_login'] = 'Invalid username or password'


class RegisterForm(forms.ModelForm):
    _username_validator = UsernameValidator()

    username = forms.CharField(
        max_length=30,
        validators=[_username_validator],
        help_text=_username_validator.get_help_text(),
    )
    password1 = forms.CharField(
        max_length=30,
        help_text=password_validation.password_validators_help_text_html(),
        widget=forms.PasswordInput,
    )
    password2 = forms.CharField(
        max_length=30,
        widget=forms.PasswordInput,
    )

    class Meta:
        model = get_user_model()
        fields = (
            'username',
            'password1',
            'password2',
        )

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        text_field_attrs = {
            'class': 'form-control width-auto',
            'autocomplete': 'off',
        }
        for f in ('username', 'password1', 'password2',):
            self.fields[f].widget.attrs.update(text_field_attrs)

        self.fields['password1'].label = 'Password'
        self.fields['password2'].label = mark_safe('Password<br/>confirmation:')

    def clean_username(self):
        username = self.cleaned_data.get('username')
        if get_user_model().objects.filter(username__iexact=username).exists():
            raise ValidationError('Username already exists..')
        return username

    def clean_password1(self):
        password = self.cleaned_data.get('password1')
        try:
            password_validation.validate_password(password, self.instance)
        except forms.ValidationError as error:
            raise ValidationError(error)
        return password

    def clean(self):
        password1 = self.cleaned_data.get('password1')
        password2 = self.cleaned_data.get('password2')
        if password1 and password1 != password2:
            raise ValidationError('Passwords don\'t match')
        return self.cleaned_data

    def save(self, commit=True):
        user = super().save(commit=False)
        user.set_password(self.cleaned_data["password1"])
        if commit:
            user.save()
        return user

