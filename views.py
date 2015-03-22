from django.http import HttpResponse
from django.utils import timezone
import datetime
from dateutil.relativedelta import relativedelta
from django.shortcuts import render

# {base}/
def index(req):
  return HttpResponse("Timelog index.")

# {base}/sheets/{cat_id}/
def sheets(req, cat_id):
  errors = []

  # GET parameters
  gp_year = req.GET.get('year', None)
  gp_month = req.GET.get('month', None)

  t_now = timezone.localtime(timezone.now())

  if (gp_year):
    try:
      year = int(gp_year)
    except ValueError:
      errors.append("Requested year invalid.")
  else:
    year = t_now.year

  if (gp_month):
    try:
      month = int(gp_month)
    except ValueError:
      errors.append("Requested month invalid.")
  else:
    month = t_now.month

  if (not errors):
    try:
      begin = datetime.datetime(year, month, 1, 0, 0, 0)
      end = begin + relativedelta(months=1)
      begin = timezone.make_aware(begin, t_now.tzinfo)
      end = timezone.make_aware(end, t_now.tzinfo)
    except ValueError as e:
      errors.append(str(e))

  res = ''
  if (errors):
    for err in errors:
      res += '<p class="err">' + err + '</p>'
    return HttpResponse(res)

  ctx = {
    # TODO: Actual category name instead of "Category <ID>" in title.
    'title': "Category %s, %s %s" \
      % (str(cat_id), begin.strftime("%B"), str(begin.year)),
    'cat_id': cat_id,
    'begin': begin,
    'end': end,
    's_begin': str(begin),
    's_end': str(end),
    'tz': timezone.get_current_timezone_name(),
  }
  return render(req, 'sheets/n.htm', ctx)
