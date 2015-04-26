# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Currently a work in progress.

## Table of Contents

* [Compatibility/requirements/dependencies](#compatibilityrequirementsdependencies)
* [Setup](#setup)
  - [Setting up a virtualenv](#setting-up-a-virtualenv)
  - [Activating the virtualenv and installing more dependencies](#activating-the-virtualenv-and-installing-more-dependencies)
  - [Proceeding with the remainder of the setup](#proceeding-with-the-remainder-of-the-setup)
* [Usage](#usage)
  - [Starting the server](#starting-the-server)
  - [Default Django admin web interface](#default-django-admin-web-interface)
  - [Time sheets](#time-sheets)
  - [Dumping data for backup and later restore](#dumping-data-for-backup-and-later-restore)
* [Updating](#updating)

## Compatibility/requirements/dependencies

All dependencies will be installed in the [setup section](#setup) below.

Most notably, we're using:

  * Django 1.8
  * Python 3.4
  * PostgreSQL 9.4

## Setup

Describing the setup procedure using Debian GNU/Linux 8.0 "Jessie".

```
# apt-get install postgresql libpq-dev python3-pip
# pip3 install virtualenv
# adduser --system --home /var/lib/timelog --group --shell /bin/bash timelog
# su - postgres
$ psql
```

```
CREATE USER timelog;
CREATE DATABASE timelog OWNER timelog;
\q
```

```
$ exit
```

### Setting up a virtualenv

```
# su - timelog
$ virtualenv-3.4 ~/venv/
```

### Activating the virtualenv and installing more dependencies

```
$ cd ~/venv/
$ source bin/activate
$ wget https://labix.org/download/python-dateutil/python-dateutil-2.0.tar.gz
$ tar xvf python-dateutil-2.0.tar.gz
$ cd python-dateutil-2.0/
$ python3 setup.py install
$ cd ../
$ pip3 install django pytz Unidecode Jinja2 psycopg2
```

### Proceeding with the remainder of the setup

```
$ django-admin startproject serve
$ cd serve/
$ git clone https://github.com/erikano/django-timelog.git timelog/
$ patch -p2 -d serve/ < timelog/patch/serve/settings.py.patch
$ patch -p2 -d serve/ < timelog/patch/serve/urls.py.patch
$ sed -i "s@\(TIME_ZONE = \)'[^']*'\$@\1'$( tzselect )'@" serve/settings.py
$ python3 manage.py makemigrations timelog
$ python3 manage.py migrate
$ python3 manage.py createsuperuser # it will suggest using name 'timelog'. Let it.
```

## Usage

### Starting the server

```
$ cd ~/venv/ \
  && source bin/activate \
  && cd serve/timelog/ \
  && python3 manage.py runserver 0.0.0.0:8000 &
```

The backgrounded process will not count as a job to the shell and
will keep running even after you log out.

### Default Django admin web interface

You *could* begin entering data into django-timelog right now at
`http://<host or IP>:8000/admin/timelog/`.
It's not great but it's better than nothing.

### Time sheets

Time sheets, though incomplete, can be retrieved from 
`http://<host or IP>:8000/timelog/hours/sheets/sheet-<slug>-<year>-<month>.<format>`, e.g.
`http://<host or IP>:8000/timelog/hours/sheets/sheet-example-2015-03.htm` or
`http://<host or IP>:8000/timelog/hours/sheets/sheet-example-2015-03.json`.

### Dumping data for backup and later restore

```
$ cd ~/venv/ \
  && source bin/activate \
  && cd serve/timelog/ \
  && python3 ../manage.py dumpdata \
       > ~/timelog-$( git describe )-dbdump-$( date +%FT%T%z ).json
```

Transfer the data dump to somewhere safe!

## Updating

```
$ cd ~/venv/ \
  && source bin/activate \
  && cd serve/timelog/ \
  && git pull \
  && python3 ../manage.py makemigrations timelog \
  && python3 ../manage.py migrate
```
