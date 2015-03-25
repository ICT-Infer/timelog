from django.conf import settings
from django.db import models
from django.utils import timezone

class Category(models.Model):
  parent = models.ForeignKey("self", null=True, blank=True)
  name = models.CharField(max_length=255)
  description = models.CharField(max_length=255, blank=True)

  def __str__(self):
    return self.name

  class Meta:
    verbose_name_plural = 'categories'

class Entry(models.Model):
  user = models.ForeignKey(settings.AUTH_USER_MODEL)
  category = models.ForeignKey(Category)
  # TODO: Validate tz_begin and tz_end are valid time zones.
  t_begin = models.DateTimeField('t_begin')
  tz_begin = models.CharField(max_length=255, default=str(timezone.get_current_timezone()))
  t_end = models.DateTimeField('t_end', null=True, blank=True)
  tz_end = models.CharField(max_length=255, default=str(timezone.get_current_timezone()))
  description = models.CharField(max_length=255, blank=True)

  def save (self, *args, **kwargs):
    self.t_begin = timezone.make_naive(self.t_begin, timezone.get_current_timezone())
    timezone.activate(self.tz_begin)
    self.t_begin = timezone.make_aware(self.t_begin, timezone.get_current_timezone())
    timezone.deactivate()

    self.t_end = timezone.make_naive(self.t_end, timezone.get_current_timezone())
    timezone.activate(self.tz_end)
    self.t_end = timezone.make_aware(self.t_end, timezone.get_current_timezone())
    timezone.deactivate()

    super(Entry, self).save(*args, **kwargs)

  def __str__(self):
    return str(self.user) + ', ' + str(self.category) + ' @ ' \
      + str(self.t_begin) + ' - ' + str(self.t_end)

  class Meta:
    verbose_name_plural = 'entries'
