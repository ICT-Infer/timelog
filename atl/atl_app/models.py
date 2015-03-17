from django.conf import settings
from django.db import models

class Project(models.Model):
  parent = models.ForeignKey("self", null=True)
  name = models.CharField(max_length=255)
  description = models.CharField(max_length=255, blank=True)

class Entry(models.Model):
  user = models.ForeignKey(settings.AUTH_USER_MODEL)
  project = models.ForeignKey(Project, null=True)
  t_begin = models.DateTimeField('t_begin', null=True)
  t_end = models.DateTimeField('t_end', null=True)
  description = models.CharField(max_length=255, blank=True)
