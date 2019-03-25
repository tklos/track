import json

from django import template
from django.urls import reverse


register = template.Library()


@register.simple_tag
def make_url(url, **kwargs):
    extra = kwargs.pop('extra', {})
    if extra == '': extra = {}

    for k, v in extra.items():
        kwargs[k] = json.dumps(v)

    return reverse(url, kwargs={**kwargs})


@register.simple_tag
def get_limited_page_range(paginator, curr_page, neighbours=10):
    num_pages = paginator.num_pages
    curr_page = min(curr_page, num_pages)

    page_begin, page_end = max(curr_page - neighbours, 1), min(curr_page + neighbours, num_pages)

    return range(page_begin, page_end+1)

