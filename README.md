# django-atl - Alternate Timelog

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Made with great help from https://docs.djangoproject.com/en/1.7/intro/tutorial01/

Currently a work in progress.

## Setup

Describing the setup procedure using Debian GNU/Linux 7.

```
# apt-get install postgresql libpq-dev python3-pip
# pip-3.2 install django psycopg2
# adduser atl
# su - postgresql
$ psql
```

```
CREATE USER atl;
CREATE DATABASE atl OWNER atl;
\q
```

```
$ ^D
# su - atl
$ django-admin.py startproject serve
$ cd serve/
$ git clone https://github.com/erikano/django-atl.git atl/
$ patch -p1 <<EOF
--- a/serve/settings.py	2015-03-18 13:23:39.205531734 +0000
+++ b/serve/settings.py	2015-03-18 13:27:48.505534237 +0000
@@ -36,6 +36,7 @@
     'django.contrib.sessions',
     'django.contrib.messages',
     'django.contrib.staticfiles',
+    'atl',
 )
 
 MIDDLEWARE_CLASSES = (
@@ -57,10 +58,13 @@
 # https://docs.djangoproject.com/en/1.7/ref/settings/#databases
 
 DATABASES = {
-    'default': {
-        'ENGINE': 'django.db.backends.sqlite3',
-        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
-    }
+    'default': {
+        'ENGINE': 'django.db.backends.postgresql_psycopg2',
+        'NAME': 'atl',
+        'USER': 'atl',
+        'PASSWORD': '',
+        'HOST': ''
+  }
 }
 
 # Internationalization
EOF
$ export EDITOR=vim # Set it to your prefered editor.
$ $EDITOR serve/settings.py # Edit TIME_ZONE.
$ python3 manage.py migrate
$ python3 manage.py createsuperuser # it will suggest using name 'atl'. Let it.
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
from atl.models import Project, Entry
from django.utils import timezone
p = Project(name='Example')
p.save()
from django.contrib.auth.models import User
u = User.objects.get(username='atl')
e = Entry(user=u, project=p)
e.save()
^D
```
