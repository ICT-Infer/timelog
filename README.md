# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Currently a work in progress.

## Compatibility/requirements/dependencies

All dependencies will be installed in the [setup section](#setup) below.

Most notably, we're using:

  * Django 1.8
  * Python 3.4
  * PostgreSQL 9.1

## Setup

Describing the setup procedure using Debian GNU/Linux 7.8 Wheezy.

```
# apt-get install postgresql libpq-dev
# adduser timelog
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

### Installing Python 3.4 from source

Jinja2 requires Python 3.3+ but the Python 3 in Debian Wheezy is Python 3.2 and I couldn't find any more recent Python 3 in backports either (though I haven't used backports before, so maybe I did something wrong). We'll install Python 3.4 from source for the `timelog' user. First we'll need to install some dependencies in order to be able to build Python.


```
# apt-get install build-essential \
                  libncurses5-dev libncursesw5-dev libreadline6-dev \
                  libdb5.1-dev libgdbm-dev libsqlite3-dev \
                  libssl-dev libbz2-dev libexpat1-dev liblzma-dev zlib1g-dev
```

Then, in a subdirectory of the home directory of the timelog user, we download the source tarball, build and install it.

```
# su - timelog
$ mkdir -p ~/opt/python3.4/
$ cd ~/opt/python3.4/
$ wget https://www.python.org/ftp/python/3.4.3/Python-3.4.3.tar.xz
$ tar xvf Python-3.4.3.tar.xz
$ cd Python-3.4.3/
$ ./configure --prefix=/home/timelog/opt/python3.4/
$ make install
```

### Setting up a virtualenv

```
$ ~/opt/python3.4/bin/pip3 install virtualenv
$ ~/opt/python3.4/bin/virtualenv ~/venv/
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
$ export EDITOR=vim # Set it to your prefered editor.
$ $EDITOR serve/settings.py # Edit TIME_ZONE.
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
