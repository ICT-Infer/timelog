from django.contrib import admin
from django.utils import timezone
from timelog.models import Category, Entry

class EntryAdmin (admin.ModelAdmin):
  def get_form (self, req, obj=None, **kwargs):
    if obj is not None:
      # Reverse of what we do in models.Entry.save.
      timezone.deactivate()
      obj.t_begin = timezone.make_naive(obj.t_begin, timezone.get_current_timezone())
      timezone.activate(obj.tz_begin)
      obj.t_begin = timezone.make_aware(obj.t_begin, timezone.get_current_timezone())

      timezone.deactivate()
      obj.t_end = timezone.make_naive(obj.t_end, timezone.get_current_timezone())
      timezone.activate(obj.tz_end)
      obj.t_end = timezone.make_aware(obj.t_end, timezone.get_current_timezone())

    return super(EntryAdmin, self).get_form(req, obj, **kwargs)

admin.site.register(Category)
admin.site.register(Entry, EntryAdmin)
