from django.contrib.auth.models import Group, Permission
from django.contrib.contenttypes.models import ContentType
from timelog.models import Category, Entry

ct_cat = ContentType.objects.get_for_model(Category)
ct_entry = ContentType.objects.get_for_model(Entry)

timelog_user_group, created = Group.objects.get_or_create(name='timelog_user')

if created:
  for ct in [ct_cat, ct_entry]:
    for permission in Permission.objects.filter(content_type=ct):
      timelog_user_group.permissions.add(permission)
