from django.contrib import admin
from django.utils import timezone
from timelog.models import Category, Entry
import pytz

class EntryAdmin (admin.ModelAdmin):
  def get_form(self, req, obj, **kwargs):
    if obj is not None and not hasattr(obj, 'tz_maniped'):
      # Reverse of what we do in models.Entry.save.
      obj.t_begin = obj.t_begin.astimezone(
        pytz.timezone(obj.tz_begin)).replace(tzinfo=None)
      obj.t_begin = timezone.make_aware(obj.t_begin,
                                        timezone.get_current_timezone())

      if obj.t_end:
        obj.t_end = obj.t_end.astimezone(pytz.timezone(obj.tz_end)).replace(
          tzinfo=None)
        obj.t_end = timezone.make_aware(obj.t_end,
                                        timezone.get_current_timezone())

      obj.tz_maniped = True

    return super(EntryAdmin, self).get_form(req, obj, **kwargs)

admin.site.register(Category)
admin.site.register(Entry, EntryAdmin)
