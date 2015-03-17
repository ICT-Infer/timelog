# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('atl_app', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='entry',
            name='description',
            field=models.CharField(max_length=255, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='entry',
            name='project',
            field=models.ForeignKey(to='atl_app.Project', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='entry',
            name='t_begin',
            field=models.DateTimeField(null=True, verbose_name='t_begin'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='entry',
            name='t_end',
            field=models.DateTimeField(null=True, verbose_name='t_end'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='project',
            name='description',
            field=models.CharField(max_length=255, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='project',
            name='parent',
            field=models.ForeignKey(to='atl_app.Project', null=True),
            preserve_default=True,
        ),
    ]
