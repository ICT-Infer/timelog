from django.conf import settings
from django.db import models

class Project(models.Model):
  parent = models.ForeignKey("self")
  name = models.CharField(max_length=255)
  description = models.CharField(max_length=255)

class Entry(models.Model):
  user = models.ForeignKey(settings.AUTH_USER_MODEL)
  project = models.ForeignKey(Project)
  t_begin = models.DateTimeField('t_begin')
  t_end = models.DateTimeField('t_end')
  description = models.CharField(max_length=255)
