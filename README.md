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
$ python3 manage.py runserver 0.0.0.0:8000 &
```
