from django.conf.urls import patterns, url
from django.contrib.auth import views as auth_views
from timelog import views

urlpatterns = patterns(
  '',
  url(r'^$', views.index, name='index'),
  url(r'^login/$', auth_views.login, {'template_name': 'login.htm'}),
  url(r'^logout/$', lambda req: auth_views.logout_then_login(req, login_url='/timelog/login/?next=' + req.GET.get('login_next', ''))),
  url(r'^hours/sheets/index-(?P<arg_year>\d+)-(?P<arg_month>\d{2})\.(?P<arg_fmt_ext>\w+)$',
    views.sheets,
    name='sheets'),
  url(r'^hours/sheets/sheet-(?P<arg_cat_slug>[0-9a-z_]+)-(?P<arg_year>\d+)-(?P<arg_month>\d{2})\.(?P<arg_fmt_ext>\w+)$',
    views.sheet,
    name='sheet'),
)
