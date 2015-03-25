from django.conf.urls import patterns, url
from timelog import views

urlpatterns = patterns('',
  url(r'^$', views.index, name='index'),
  url(r'^hours/sheets/index-(?P<arg_year>\d+)-(?P<arg_month>\d{2})\.(?P<arg_fmt_ext>\w+)$', views.sheets, name='sheets'),
  url(r'^hours/sheets/sheet-(?P<arg_cat_slug>[0-9a-z_]+)-(?P<arg_year>\d+)-(?P<arg_month>\d{2})\.(?P<arg_fmt_ext>\w+)$', views.sheet, name='sheet'),
)
