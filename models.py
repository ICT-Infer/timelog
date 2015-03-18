from django.conf import settings
from django.db import models

class Category(models.Model):
  parent = models.ForeignKey("self", null=True)
  name = models.CharField(max_length=255)
  description = models.CharField(max_length=255, blank=True)

  def __str__(self):
    return self.name

  class Meta:
    verbose_name_plural = 'categories'

class Entry(models.Model):
  user = models.ForeignKey(settings.AUTH_USER_MODEL)
  category = models.ForeignKey(Category, null=True)
  t_begin = models.DateTimeField('t_begin', null=True)
  t_end = models.DateTimeField('t_end', null=True)
  description = models.CharField(max_length=255, blank=True)

  def __str__(self):
    return str(self.user) + ', ' + str(self.category) + ' @ ' \
      + str(self.t_begin) + ' - ' + str(self.t_end)

  class Meta:
    verbose_name_plural = 'entries'
