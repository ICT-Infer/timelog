import sys
from timelog import views
from django.test.client import RequestFactory

rf = RequestFactory()
get_request = rf.get('/timelog/hours/sheets/sheet-lokalavisen_skravlebotta-2015-03.json')

try:
  views.sheet(get_request, 'lokalavisen_skravlebotta', '2015', '03', 'json')
except Exception as e:
  print(e, file=sys.stderr)
  sys.exit(1)
