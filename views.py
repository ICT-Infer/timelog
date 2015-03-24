from django.http import HttpResponse, JsonResponse
from django.utils import timezone
import datetime
from dateutil.relativedelta import relativedelta
from django.shortcuts import render
from timelog.models import Category, Entry

# {base}/
def index(req):
  return HttpResponse("Timelog index.")

def sheets_format_htm (req, ctx):
  if 'errors' in ctx['view_data'] and ctx['view_data']['errors']:
    res = ''
    for err in ctx['view_data']['errors']:
      res += '<p class="err">' + err + '</p>'
    return HttpResponse(res, status=500)
  return render(req, 'sheets/tl-cat_id-year-month.htm', ctx)

def sheets_format_json (req, ctx):
  return JsonResponse(ctx['view_data'])

def sheets_format_dispatcher (req, ctx, arg_fmt_ext):
  if arg_fmt_ext == 'htm':
    return sheets_format_htm(req, ctx)
  elif arg_fmt_ext == 'json':
    return sheets_format_json(req, ctx)
  else:
    res_str = "Unknown file format extension `.%s'." % arg_fmt_ext
    return HttpResponse(res_str, status=404)

# {base}/hours/sheets/sheet-{cat_id}-{year}-{month}.htm
def sheets(req, arg_cat_id, arg_year, arg_month, arg_fmt_ext):
  cat_id = int(arg_cat_id)
  year = int(arg_year)
  month = int(arg_month)

  errors = []

  # Options from GET parameters
  # TODO: Handle invalid values.
  # TODO: Separate input handling from rest of function.
  opt = {}
  opt['no_recurse'] = int(req.GET.get('no-recurse', 0))
  opt['own_only'] = int(req.GET.get('own-entries-only', 0))
  opt['grp_cat'] = int(req.GET.get('group-by-category', 0))
  opt['grp_psn'] = int(req.GET.get('group-by-person', 0))
  opt['grp_week'] = int(req.GET.get('group-by-week', 0))

  t_now = timezone.localtime(timezone.now())

  try:
    begin = datetime.datetime(year, month, 1, 0, 0, 0)
    end = begin + relativedelta(months=1)
    begin = timezone.make_aware(begin, t_now.tzinfo)
    end = timezone.make_aware(end, t_now.tzinfo)
  except ValueError as e:
    errors.append(str(e))

  view_data = {}
  ctx_tmp = {}

  if (not errors):
    # TODO: Limit entries to current range.
    # TODO: Grouping
    # TODO: Sorting
    # TODO: Splitting

    db_entries = Entry.objects.filter()

    v_entries = []

    for db_entry in db_entries:
      v_entry = {}
      v_entry['date'] = db_entry.t_begin.strftime("%Y-%m-%d")
      v_entry['t_begin'] = db_entry.t_begin.strftime("%H:%M:%S")
      v_entry['t_end'] = db_entry.t_end.strftime("%H:%M:%S")
      v_entry['category'] = str(db_entry.category)
      v_entry['user'] = str(db_entry.user)
      v_entries.append(v_entry)

    try:
      # TODO: Actual category name instead of "Category <ID>" in title.
      ctx_tmp['title'] = "Category %s, %s %s" \
          % (str(cat_id), begin.strftime("%B"), str(begin.year))
    except ValueError as e:
      errors.append(str(e))

    ctx_tmp['query_str'] = req.GET.urlencode()
    ctx_tmp['opt'] = opt
    ctx_tmp['cat_id'] = cat_id
    ctx_tmp['begin'] = begin
    ctx_tmp['end'] = end
    ctx_tmp['s_begin'] = str(begin)
    ctx_tmp['s_end'] = str(end)
    ctx_tmp['s_now'] = str(t_now)
    ctx_tmp['tz'] = timezone.get_current_timezone_name()

  if (errors):
    view_data = {}
    view_data['status'] = 'error'
    view_data['errors'] = errors
    ctx_tmp = {'view_data': view_data}
  else:
    view_data['status'] = 'ok'
    view_data['entries'] = v_entries
    ctx_tmp['view_data'] = view_data

  ctx = ctx_tmp

  return sheets_format_dispatcher(req, ctx, arg_fmt_ext)
