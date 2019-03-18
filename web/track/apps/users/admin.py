from django.contrib import admin
from django.db.models.functions import Lower
from django.contrib.auth.admin import UserAdmin as DjangoUserAdmin
from django.contrib.auth.models import Group

from .models import User


@admin.register(User)
class UserAdmin(DjangoUserAdmin):
    ordering = [
        Lower('username'),
    ]
    add_form_template = 'admin/change_form.html'
    add_fieldsets = (
        (
            None, {
                'fields': (
                    'username',
                    'email',
                    'password1',
                    'password2',
                ),
            }
        ),
    )
    fieldsets = None
    list_display = (
        'username',
        'email',
        'is_superuser',
    )
    list_display_links = list_display
    list_filter = (
        'is_superuser',
    )
    fields = (
        'username',
        'password',
        'email',
        'is_superuser',
        'date_joined',
    )
    readonly_fields_create = (
    )
    readonly_fields_modify = (
        'username',
        'email',
    )

    def get_readonly_fields(self, request, obj=None):
        return self.readonly_fields_create if obj is None else self.readonly_fields_modify


admin.site.unregister(Group)

