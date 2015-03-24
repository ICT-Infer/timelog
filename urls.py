from django.conf.urls import patterns, url
from timelog import views

urlpatterns = patterns('',
  url(r'^$', views.index, name='index'),
  url(r'^hours/sheets/sheet-(?P<arg_cat_id>\d+)-(?P<arg_year>\d+)-(?P<arg_month>\d{2})\.htm$', views.sheets, name='sheets'),
)
