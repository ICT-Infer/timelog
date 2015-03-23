from django.http import HttpResponse
from django.utils import timezone
import datetime
from dateutil.relativedelta import relativedelta
from django.shortcuts import render

# {base}/
def index(req):
  return HttpResponse("Timelog index.")

# {base}/sheets/tl-{cat_id}-{year}-{month}.htm
def sheets(req, arg_cat_id, arg_year, arg_month):
  cat_id = int(arg_cat_id)
  year = int(arg_year)
  month = int(arg_month)

  errors = []

  # Options from GET parameters
  # TODO: Handle invalid values.
  # TODO: Separate input handling from rest of function.
  opt = {}
  opt['no-default-true'] = int(req.GET.get('no-default-true', 0))
  opt_default_true = True and not opt['no-default-true']
  opt['recurse'] = int(req.GET.get('recurse', opt_default_true))
  opt['own_only'] = int(req.GET.get('own-entries-only', 0))
  opt['grp_cat'] = int(req.GET.get('group-by-category', 0))
  opt['grp_psn'] = int(req.GET.get('group-by-person', 0))
  opt['grp_week'] = int(req.GET.get('group-by-week', opt_default_true))

  t_now = timezone.localtime(timezone.now())

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
    'req': req,
    'query_str': req.GET.urlencode(),
    'opt': opt,
    'title': "Category %s, %s %s" \
      % (str(cat_id), begin.strftime("%B"), str(begin.year)),
    'cat_id': cat_id,
    'begin': begin,
    'end': end,
    's_begin': str(begin),
    's_end': str(end),
    'tz': timezone.get_current_timezone_name(),
  }
  return render(req, 'sheets/tl-cat_id-year-month.htm', ctx)
