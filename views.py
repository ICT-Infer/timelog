from django.http import HttpResponse, JsonResponse
from django.utils import timezone
import datetime
from dateutil.relativedelta import relativedelta
from django.shortcuts import render
from timelog.models import Category, Entry
from django.db.models import Q

#
# Shared functions common to other view functions.
#

def category_tree (arg_year, arg_month, arg_fmt_ext, arg_root=None):
  tree = []

  for cat in Category.objects.filter(parent=arg_root).order_by('name'):
    # TODO: Only include sum_hours if it has non-zero value.
    cat_branch = {'id': cat.id, 'name': cat.name, 'description': cat.description, 'slug': cat.slug, 'sum_hours': "XX:XX", 'details': "sheet-" + cat.slug \
                 + "-" + arg_year + "-" + arg_month \
                 + "." + arg_fmt_ext, }
    cat_branch['children'] = \
      category_tree(arg_year, arg_month, arg_fmt_ext, cat.id)
    if cat_branch['children']:
      # TODO: Only include rec_sum_hours if greater than sum_hours
      cat_branch['rec_sum_hours'] = "XX:XX"

    tree.append(cat_branch)

  return tree

def flatten_tree (tree):
  flat = []
  for cat in tree:
    flat.append(cat['id'])
    flat += flatten_tree(cat['children'])
  return flat

#
# View functions and their subfunctions
#

# {base}/
def index (req):
  return HttpResponse("Timelog index.")

# {base}/hours/sheets/index-{year}-{month}.{fmt_ext}
def sheets (req, arg_year, arg_month, arg_fmt_ext):

  view_data = {}
  view_data['category_tree'] = category_tree(arg_year, arg_month, arg_fmt_ext)

  ctx = {'view_data': view_data, }
  return render(req, 'hours/sheets/index-year-month.htm', ctx)

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

  cat = Category.objects.get(slug=cat_slug)
  cat_id = cat.id
  cat_name = cat.name

  errors = []

  # Options from GET parameters
  # TODO: Handle invalid values.
  # TODO MAYBE: Make opts no_recurse and grp_cat mutually exclusive?
  # TODO MAYBE: Make opts own_only and grp_psn mutually exclusive?
  # TODO: Separate input handling from rest of function.
  opt = {}
  opt['no_recurse'] = int(req.GET.get('no-recurse', 0))
  opt['own_only'] = int(req.GET.get('own-entries-only', 0))
  opt['grp_cat'] = int(req.GET.get('group-by-category', 0))
  opt['grp_psn'] = int(req.GET.get('group-by-person', 0))
  opt['grp_week'] = int(req.GET.get('group-by-week', 0))

  t_now = timezone.localtime(timezone.now())

  try:
    t_lower_bound_incl = datetime.datetime(year, month, 1, 0, 0, 0)
    t_upper_bound_excl = t_lower_bound_incl + relativedelta(months=1)
    t_lower_bound_incl = timezone.make_aware(t_lower_bound_incl, t_now.tzinfo)
    t_upper_bound_excl = timezone.make_aware(t_upper_bound_excl, t_now.tzinfo)
  except ValueError as e:
    errors.append(str(e))

  view_data = {}
  ctx_tmp = {}

  cats = [cat_id]
  if not opt['no_recurse']:
    cats += flatten_tree(category_tree(arg_year, arg_month, arg_fmt_ext, cat_id))
  ctx_tmp['cats'] = cats

  if (not errors):
    # TODO: Grouping

    db_entries = Entry.objects.filter(
      Q(category__in = cats) &
      ((Q(t_begin__gte = t_lower_bound_incl)
        & Q(t_begin__lt = t_upper_bound_excl))
      | (Q(t_end__gte = t_lower_bound_incl)
        & Q(t_end__lt = t_upper_bound_excl)))
    )

    v_entries = []

    for db_entry in db_entries:
      entries_split_local = db_entry.in_localtime()\
                           .limited_to_bounds(t_lower_bound_incl,
                                              t_upper_bound_excl)\
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

        v_entries.append(v_entry)

    # Sorting on string representation is not so pretty but it's convenient.
    v_entries.sort(
      key = lambda e: e['date'] + "T" + e['t_begin']
                                + "-" + (e['t_end'] or "None"))

    # TODO: Sum for each kind of grouping
    sdts = sum([e['duration'] for e in v_entries if e['duration']],
               datetime.timedelta()).total_seconds()
    v_sum_duration = "%02d:%02d:%02d" \
                     % (sdts // 3600, (sdts % 3600) // 60, sdts % 60)

    # Turn durations into strings so they're JSON serializeable.
    for e in v_entries:
      e['duration'] = (str(e['duration']) if e['duration'] else None)

    try:
      ctx_tmp['title'] = "%s, %s %s" \
          % (cat_name, t_lower_bound_incl.strftime("%B"),
             t_lower_bound_incl.year)
    except ValueError as e:
      errors.append(str(e))

    ctx_tmp['redir'] = {}
    ctx_tmp['redir']['sheet'] = "/timelog/redir/sheet.htm"
    ctx_tmp['query_str'] = req.GET.urlencode()
    ctx_tmp['opt'] = opt
    ctx_tmp['cat_id'] = cat_id
    ctx_tmp['cat_name'] = cat_name
    ctx_tmp['t_lower_bound_incl'] = t_lower_bound_incl
    ctx_tmp['t_upper_bound_excl'] = t_upper_bound_excl
    ctx_tmp['t_now'] = t_now
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
