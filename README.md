# django-timelog

Django Timelog App. Timesheet time tracking for a person or group of people.

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Made with great help from https://docs.djangoproject.com/en/1.7/intro/tutorial01/

Currently a work in progress.

## Setup

Describing the setup procedure using Debian GNU/Linux 7.

```
# apt-get install postgresql libpq-dev python3-pip
# pip-3.2 install django psycopg2
# adduser timelog
# su - postgresql
$ psql
```

```
CREATE USER timelog;
CREATE DATABASE timelog OWNER timelog;
\q
```

```
$ ^D
# su - timelog
$ django-admin.py startproject serve
$ cd serve/
$ git clone https://github.com/erikano/django-timelog.git timelog/
$ patch -p1 <<EOF
--- a/serve/settings.py	2015-03-19 00:40:02.253528784 +0000
+++ b/serve/settings.py	2015-03-19 00:42:52.749530743 +0000
@@ -36,6 +36,7 @@
     'django.contrib.sessions',
     'django.contrib.messages',
     'django.contrib.staticfiles',
+    'timelog',
 )
 
 MIDDLEWARE_CLASSES = (
@@ -58,8 +59,11 @@
 
 DATABASES = {
     'default': {
-        'ENGINE': 'django.db.backends.sqlite3',
-        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
+        'ENGINE': 'django.db.backends.postgresql_psycopg2',
+        'NAME': 'timelog',
+        'USER': 'timelog',
+        'PASSWORD': '',
+        'HOST': ''
     }
 }
 
EOF
$ export EDITOR=vim # Set it to your prefered editor.
$ $EDITOR serve/settings.py # Edit TIME_ZONE.
$ python3 manage.py makemigrations timelog
$ python3 manage.py migrate
$ python3 manage.py createsuperuser # it will suggest using name 'timelog'. Let it.
$ python3 manage.py runserver 0.0.0.0:8000 &
```

## Usage from command-line

Since the web UI has not yet been created, here is some initial basic usage.

This continues in the session that we began above when we did the setup.

We are not yet dealing with other users.

```
$ python3 manage.py shell
```

```
from timelog.models import Category, Entry
from django.utils import timezone
c = Category(name='Example')
c.save()
from django.contrib.auth.models import User
u = User.objects.get(username='timelog')
e = Entry(user=u, category=c, t_begin=timezone.now())
e.save()
^D
```

## Usage with default Django admin web interface

You *could* begin entering data into django-timelog right now at
`http://<host or IP>:8000/admin/timelog/`.
It's not great but it's better than nothing
and better than using it through the shell like above.

## Updating

```
$ cd ~/serve/timelog \
  && git pull \
  && python3 ../manage.py makemigrations timelog
```
