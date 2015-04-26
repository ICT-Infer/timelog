# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

An alternative to my [saas-by-erik/timelog](https://github.com/saas-by-erik/timelog).

Currently a work in progress.

## Table of Contents

* [Compatibility/requirements/dependencies](#compatibilityrequirementsdependencies)
* [Installation](#installation)
* [Usage](#usage)
  - [Starting the server](#starting-the-server)
  - [Default Django admin web interface](#default-django-admin-web-interface)
  - [Time sheets](#time-sheets)
  - [Dumping data for backup and later restore](#dumping-data-for-backup-and-later-restore)
* [Updating](#updating)

## Compatibility/requirements/dependencies

All dependencies will be installed during [installation](#installation).

Most notably, we're using:

  * Django 1.8
  * Python 3.4
  * PostgreSQL 9.4

## Installation

For Debian GNU/Linux 8.0 "Jessie", an install script is provided.

Please give the install script a read-through after you've downloaded it
and prior to running it.

```
wget https://raw.githubusercontent.com/erikano/django-timelog/master/installer.sh
sudo bash ./installer.sh
```

When you run the install script, you will be asked to pick a time zone.
Please note that this will *not* affect your Debian system time zone --
the value you pick will only be used in the Django project created
for the Django Timelog App.

Also, during the install script run, you will be asked for a username
which you should leave blank to accept the default value; 'timelog'.
It will also ask for an e-mail address for that user which I leave blank.
Finally, it will ask for a password for the user.
The user created is the one you will log into the Django Admin web-UI with
so that you can administer Django accounts for you users.

## Usage

### Starting the server

```
sudo -u timelog -i -- bash -c "source ~/venv/bin/activate \
  && python3 ~/venv/serve/manage.py runserver 0.0.0.0:8000 &"
```

The backgrounded process will keep running even after you log out.

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
sudo -u timelog -i -- bash -c "source ~/venv/bin/activate \
  && python3 ~/venv/serve/manage.py dumpdata \
       > ~/timelog-$( git describe )-dbdump-$( date +%FT%T%z ).json"
```

Transfer the data dump to somewhere safe!

## Updating

```
sudo -u timelog -i -- bash -c "source ~/venv/bin/activate \
  && cd ~/venv/serve/timelog/ \
  && git pull \
  && python3 ../manage.py makemigrations timelog \
  && python3 ../manage.py migrate"
```
