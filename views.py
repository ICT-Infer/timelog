from django.http import HttpResponse, JsonResponse
from django.utils import timezone
import datetime
from dateutil.relativedelta import relativedelta
from django.shortcuts import render
from timelog.models import Category, Entry
from django.db.models import Q
import itertools

#
# Shared functions common to other view functions.
#

def entries (arg_datetime_bounds, arg_cat_id):

  db_entries = Entry.objects.filter(
    Q(category = arg_cat_id) &
    ((Q(t_begin__gte = arg_datetime_bounds['lower_incl'])
      & Q(t_begin__lt = arg_datetime_bounds['upper_excl']))
    | (Q(t_end__gte = arg_datetime_bounds['lower_incl'])
      & Q(t_end__lt = arg_datetime_bounds['upper_excl'])))
  )

  for db_entry in db_entries:
    entries_split_local = db_entry.in_localtime()\
                         .limited_to_bounds(arg_datetime_bounds)\
                         .split_on_midnight()

    for i, entry in enumerate(entries_split_local):
      v_entry = {
        # "-lpt-" in id means "local part", as in part within the date bounds
        'id':          "e-" + str(entry.pk) + "-lpt-" + str(i),
        'date':        entry.t_begin.strftime("%F"),
        't_begin':     entry.t_begin.strftime("%T"),
        'category':    str(db_entry.category),
        'user':        str(db_entry.user),
        'description': db_entry.description,
      }

      if entry.t_end:
        v_entry['t_end']    = entry.t_end.strftime("%T")
        # Rounded duration
        d = entry.t_end - entry.t_begin
        v_entry['duration'] = d - datetime.timedelta(
          seconds=(-1 if (d.microseconds >= 500) else 0),
          microseconds=d.microseconds)
      else:
        v_entry['t_end']    = None
        v_entry['duration'] = None

      yield v_entry


def node (arg_datetime_bounds, arg_fmt_ext, arg_cat):

  return \
    {
      'id': arg_cat.id,
      'name': arg_cat.name,
      'description': arg_cat.description,
      'slug': arg_cat.slug,
      'details': "sheet" \
                 + "-" + arg_cat.slug \
                 + "-" + str(arg_datetime_bounds['lower_incl'].year) \
                 + "-" + "%02d" % arg_datetime_bounds['lower_incl'].month \
                 + "." + arg_fmt_ext,
      'entries': entries(arg_datetime_bounds, arg_cat.id),
      'sum_hours': "XX:XX", # TODO
      'children': tree(arg_datetime_bounds, arg_fmt_ext, arg_cat.id),
      'rec_sum_hours': "XX:XX", # TODO
    }


def tree (arg_datetime_bounds, arg_fmt_ext, arg_root=None):

  for cat in Category.objects.filter(parent=arg_root).order_by('name'):
    yield node(arg_datetime_bounds, arg_fmt_ext, cat)


def flattened (tree):

  for cat in tree:
    yield cat
    yield from flattened(cat['children'])


def ym_bounds (arg_year, arg_month):

  now = timezone.localtime(timezone.now())

  try:
    lbound_incl = datetime.datetime(arg_year, arg_month, 1, 0, 0, 0)
    ubound_excl = lbound_incl + relativedelta(months=1)
    lbound_incl = timezone.make_aware(lbound_incl, now.tzinfo)
    ubound_excl = timezone.make_aware(ubound_excl, now.tzinfo)
  except ValueError as e:
    errors.append(str(e))

  return {'lower_incl': lbound_incl, 'upper_excl': ubound_excl}


#
# View functions and their subfunctions
#

# {base}/
def index (req):

  return HttpResponse("Timelog index.")


# {base}/hours/sheets/index-{year}-{month}.{fmt_ext}
def sheets (req, arg_year, arg_month, arg_fmt_ext):

  view_data = {}
  bounds = ym_bounds(int(arg_year), int(arg_month))
  view_data['category_tree'] = tree(bounds, arg_fmt_ext)

  ctx = {'view_data': view_data, }

  if arg_fmt_ext == 'htm':
    return render(req, 'hours/sheets/index-year-month.htm', ctx)
  elif arg_fmt_ext == 'json':
    # TODO: Fix generator object is not JSON serializable
    return JsonResponse(ctx['view_data'])
  # TODO: Else, error


def sheet_format_htm (req, ctx):

  if 'errors' in ctx['view_data'] and ctx['view_data']['errors']:
    res = ''
    for err in ctx['view_data']['errors']:
      res += '<p class="err">' + err + '</p>'
    return HttpResponse(res, status=500)
  return render(req, 'hours/sheets/sheet-cat_slug-year-month.htm', ctx)


def sheet_format_json (req, ctx):

  return JsonResponse(ctx['view_data'])


def sheet_format_dispatcher (req, ctx, arg_fmt_ext):

  if arg_fmt_ext == 'htm':
    return sheet_format_htm(req, ctx)
  elif arg_fmt_ext == 'json':
    return sheet_format_json(req, ctx)
  else:
    res_str = "Unknown file format extension `.%s'." % arg_fmt_ext
    return HttpResponse(res_str, status=404)


# {base}/hours/sheets/sheet-{cat_slug}-{year}-{month}.{fmt_ext}
def sheet (req, arg_cat_slug, arg_year, arg_month, arg_fmt_ext):

  cat_slug = arg_cat_slug
  year = int(arg_year)
  month = int(arg_month)

  # TODO: Error if arg_fmt_ext is neither htm nor json.
  #       Either take care of it here or some other place.

  bounds = ym_bounds(int(arg_year), int(arg_month))

  # TODO: Rename "cat"
  cat = node(bounds, arg_fmt_ext, Category.objects.get(slug=cat_slug))

  errors = []

  # Options from GET parameters
  # TODO: Handle invalid values.
  # TODO: Enforce mutual exlusivity of opts no_recurse and grp_cat.
  # TODO: Enforce mutual exlusivity of opts own_only and grp_psn.
  # TODO: Separate input handling from rest of function.
  opt = {}
  opt['no_recurse'] = int(req.GET.get('no-recurse', 0))
  opt['own_only'] = int(req.GET.get('own-entries-only', 0))
  opt['grp_cat'] = int(req.GET.get('group-by-category', 0))
  opt['grp_psn'] = int(req.GET.get('group-by-person', 0))
  opt['grp_week'] = int(req.GET.get('group-by-week', 0))

  datetime_now = timezone.localtime(timezone.now())

  view_data = {}
  ctx_tmp = {}

  cats = [cat]
  if not opt['no_recurse']:
    cats = itertools.chain(cats, flattened(tree(bounds, arg_fmt_ext, cat['id'])))
  ctx_tmp['cats'] = cats

  if (not errors):
    # TODO: Grouping

    v_entries = []

    for c in cats:
      for v_entry in c['entries']:
        v_entries.append(v_entry)

    # Sorting on string representation is not so pretty but it's convenient.
    v_entries.sort(
      key = lambda e: e['date'] + "T" + e['t_begin']
                                + "-" + (e['t_end'] or "None"))

    # TODO: Sum for each kind of grouping
    sdts = sum([e['duration'] for e in v_entries if e['duration']],
               datetime.timedelta()).total_seconds()
    v_sum_duration = "%d:%02d:%02d" \
                     % (sdts // 3600, (sdts % 3600) // 60, sdts % 60)

    # Turn durations into strings so they're JSON serializeable.
    for e in v_entries:
      e['duration'] = (str(e['duration']) if e['duration'] else None)

    try:
      ctx_tmp['title'] = "%s, %s %s" \
          % (cat['name'],
             bounds['lower_incl'].strftime("%B"),
             bounds['lower_incl'].year)
    except ValueError as e:
      errors.append(str(e))

    ctx_tmp['redir'] = {}
    ctx_tmp['redir']['sheet'] = "/timelog/redir/sheet.htm"
    ctx_tmp['query_str'] = req.GET.urlencode()
    ctx_tmp['opt'] = opt
    ctx_tmp['cat_id'] = cat['id']
    ctx_tmp['cat_name'] = cat['name']
    ctx_tmp['datetime_lbound_incl'] = bounds['lower_incl']
    ctx_tmp['datetime_ubound_excl'] = bounds['upper_excl'] 
    ctx_tmp['t_now'] = datetime_now
    ctx_tmp['tz'] = timezone.get_current_timezone_name()

  if (errors):
    view_data = {}
    view_data['status'] = 'error'
    view_data['errors'] = errors
    ctx_tmp = {'view_data': view_data}
  else:
    view_data['status'] = 'ok'
    view_data['overview'] = "index-" + arg_year \
                            + "-" + arg_month + "." + arg_fmt_ext
    view_data['entries'] = v_entries
    view_data['sum_duration'] = v_sum_duration
    ctx_tmp['view_data'] = view_data
    ctx_tmp['req_path'] = req.path

  ctx = ctx_tmp

  return sheet_format_dispatcher(req, ctx, arg_fmt_ext)
