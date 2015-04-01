# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Made with great help from https://docs.djangoproject.com/en/1.8/intro/tutorial01/

Currently a work in progress.

## Compatibility, requirements, dependencies

Compatible with Django 1.8+. Requires Python 3.

Other dependencies are listed in the *setup* section below.

## Setup

Describing the setup procedure using Debian GNU/Linux 7.

```
# apt-get install python3-dateutil python3-pip postgresql libpq-dev
# pip-3.2 install django pytz Unidecode psycopg2
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
$ exit
# su - timelog
$ django-admin startproject serve
$ cd serve/
$ git clone https://github.com/erikano/django-timelog.git timelog/
$ patch -p2 -d serve/ < timelog/patch/serve/settings.py.patch
$ patch -p2 -d serve/ < timelog/patch/serve/urls.py.patch
$ export EDITOR=vim # Set it to your prefered editor.
$ $EDITOR serve/settings.py # Edit TIME_ZONE.
$ python3 manage.py makemigrations timelog
$ python3 manage.py migrate
$ python3 manage.py createsuperuser # it will suggest using name 'timelog'. Let it.
$ python3 manage.py runserver 0.0.0.0:8000 &
```

## Usage with default Django admin web interface

You *could* begin entering data into django-timelog right now at
`http://<host or IP>:8000/admin/timelog/`.
It's not great but it's better than nothing.

## Time sheets

Time sheets, though incomplete, can be retrieved from 
`http://<host or IP>:8000/timelog/hours/sheets/sheet-<slug>-<year>-<month>.<format>`, e.g.
`http://<host or IP>:8000/timelog/hours/sheets/sheet-example-2015-03.htm` or
`http://<host or IP>:8000/timelog/hours/sheets/sheet-example-2015-03.json`.

## Updating

```
$ cd ~/serve/timelog \
  && git pull \
  && python3 ../manage.py makemigrations timelog \
  && python3 ../manage.py migrate
```
