# atl - Alternate Timelog

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
$ git clone https://github.com/erikano/atl.git
$ cd atl/atl/
$ export EDITOR=vim # Set it to your prefered editor.
$ $EDITOR atl/settings.py # Edit SECRET_KEY and TIME_ZONE.
$ python3 manage.py migrate
$ python3 manage.py createsuperuser # it will suggest naming the superuser 'atl'. Let it.
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
from atl_app.models import Project, Entry
from django.utils import timezone
p = Project(name='Example')
p.save()
from django.contrib.auth.models import User
u = User.objects.get(username='atl')
e = Entry(user=u, project=p)
e.save()
^D
```
