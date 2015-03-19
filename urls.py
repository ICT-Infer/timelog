from django.conf.urls import patterns, url
from timelog import views

urlpatterns = patterns('',
  url(r'^$', views.index, name='index'),
  url(r'^sheets/(?P<cat_id>\d+)/$', views.sheets, name='sheets'),
)
