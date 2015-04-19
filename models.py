from django.conf import settings
from django.db import models
from unidecode import unidecode
from django.template.defaultfilters import slugify
import re
from django.utils import timezone
from django.core.exceptions import ValidationError
import pytz
from dateutil.relativedelta import relativedelta

class Category (models.Model):
  parent = models.ForeignKey("self", null=True, blank=True)
  name = models.CharField(max_length=255)
  description = models.CharField(max_length=255, blank=True)
  slug = models.SlugField(editable=False, unique=True)

  def save (self, *args, **kwargs):
    if (not self.id):
      self.slug = re.sub('_{2,}', '_',
                         slugify(unidecode(self.name)).replace('-', '_'))
      if (self.slug == ''):
        # Not a very good default slug name but can't think of any better.
        self.slug = "default_slug"

      i = 2
      slug_cmp = self.slug
      try:
        slug_dupe = Category.objects.get(slug=slug_cmp)
        while (slug_dupe):
          slug_cmp = self.slug + '_' + str(i)
          slug_dupe = Category.objects.get(slug=slug_cmp)
          i += 1
      except Category.DoesNotExist:
        pass

      self.slug = slug_cmp

    super(Category, self).save(*args, **kwargs)

  def __str__ (self):
    return self.name

  class Meta:
    verbose_name_plural = 'categories'

class Entry (models.Model):
  user = models.ForeignKey(settings.AUTH_USER_MODEL)
  category = models.ForeignKey(Category)
  # TODO: Validate tz_begin and tz_end are valid time zones.
  t_begin = models.DateTimeField('t_begin')
  tz_begin = models.CharField(max_length=255,
                              default=str(timezone.get_current_timezone()))
  t_end = models.DateTimeField('t_end', null=True, blank=True)
  tz_end = models.CharField(max_length=255,
                            default=str(timezone.get_current_timezone()))
  description = models.CharField(max_length=255, blank=True)

  def clean (self):
    # Validation of the model as a whole

    if (self.t_begin.tzinfo.utcoffset(None) !=
        timezone.get_current_timezone().utcoffset(None)):
      raise ValidationError("t_begin.tzinfo.utcoffset(None) must be equal to " \
                            + "timezone.get_current_timezone().utcoffset(None).")
    if (self.t_end and self.t_end.tzinfo.utcoffset(None) !=
        timezone.get_current_timezone().utcoffset(None)):
      raise ValidationError("t_end.tzinfo.utcoffset(None) must be equal to " \
                            + "timezone.get_current_timezone().utcoffset(None).")

    if (self.t_end and self.t_end < self.t_begin):
      raise ValidationError(
        "t_end shall not be < t_begin when t_end is not None")

    super(Entry, self).clean()

  def save (self, *args, **kwargs):
    self.t_begin = timezone.make_naive(self.t_begin,
                                       timezone.get_current_timezone())
    timezone.activate(self.tz_begin)
    self.t_begin = timezone.make_aware(self.t_begin,
                                       timezone.get_current_timezone())
    timezone.deactivate()

    if (self.t_end):
      self.t_end = timezone.make_naive(self.t_end,
                                       timezone.get_current_timezone())
      timezone.activate(self.tz_end)
      self.t_end = timezone.make_aware(self.t_end,
                                       timezone.get_current_timezone())
      timezone.deactivate()

    super(Entry, self).save(*args, **kwargs)

  def __str__ (self):
    retstr = str(self.user) + ', ' + str(self.category) + ' @ ' \
      + str(self.t_begin.astimezone(pytz.timezone(self.tz_begin))) + ' (' + self.tz_begin + ') - '
    if (self.t_end):
      retstr += str(self.t_end.astimezone(
        pytz.timezone(self.tz_end))) + ' (' + self.tz_end + ')'
    else:
      retstr += str(None) + ' (' + self.tz_end + ')'

    return retstr

  class Meta:
    verbose_name_plural = 'entries'

  #
  # Transformed copies (might not be the best description for these).
  # TODO: Prevent transformed copies from being saved.
  # TODO MAYBE: Enable choosing time zone for these functions.
  #

  def in_localtime (self):
    t_begin = timezone.localtime(self.t_begin)
    tz_begin = str(timezone.get_current_timezone())
    if self.t_end:
      t_end = timezone.localtime(self.t_end)
    else:
      t_end = None
    tz_end = str(timezone.get_current_timezone())

    return Entry(user = self.user, category = self.category,
                 t_begin = t_begin, tz_begin = tz_begin,
                 t_end = t_end, tz_end = tz_end,
                 description = self.description)

  def limited_to_bounds (self, t_lower_bound_incl, t_upper_bound_excl):
    if self.t_begin < t_lower_bound_incl:
      t_begin = t_lower_bound_incl
    else:
      t_begin = self.t_begin
    if self.t_end:
      if self.t_end > t_upper_bound_excl:
        t_end = t_upper_bound_excl - relativedelta(microseconds=1)
      else:
        t_end = self.t_end
    else:
       t_end = None

    return Entry(user = self.user, category = self.category,
                 t_begin = t_begin, tz_begin = self.tz_begin,
                 t_end = t_end, tz_end = self.tz_end,
                 description = self.description)
