# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

Currently a work in progress.

## Table of Contents

* [Supported platforms](#supported-platforms)
* [Dependencies](#dependencies)
* [Installation](#installation)
* [Usage](#usage)
  - [Stopping, starting and restarting the server](#stopping-starting-and-restarting-the-server)
  - [Default Django site admin web interface](#default-django-site-admin-web-interface)
    + [Adding users](#adding-users)
    + [Entering data](#entering-data)
  - [Timelog web UI](#timelog-web-ui)
  - [Dumping data for backup and later restore](#dumping-data-for-backup-and-later-restore)
* [Updating and upgrading](#updating-and-upgrading)
* [Copyright and license](#copyright-and-license)

## Supported platforms

* Debian GNU/Linux 8.0 "Jessie"

## Dependencies

Most notable dependencies:

  * nginx
  * Django 1.8
  * Python 3.4
  * PostgreSQL 9.4

All dependencies will be installed during [installation](#installation).

## Installation

For [supported platforms](#supported-platforms), an install script is provided.
(A corresponding uninstall script is provided as well, should you ever want to
remove timelog. You shouldn't want to remove it though, right?)

When you run the install script, you will be asked to pick a time zone.
Please note that this will *not* affect your system time zone --
the value you pick will only be used in the Django project created
for the Django Timelog App.

Also, during the install script run, you will be asked for a password
to use for the timelog Django superuser account. See also:
[Django site admin web interface](#default-django-site-admin-web-interface).

Please give the install script a read-through after you've downloaded it
and prior to running it.

```
wget https://raw.githubusercontent.com/erikano/django-timelog/master/scripts/timelog-install.bash
sudo bash ./timelog-install.bash
```

Once the install script has finished, enable the timelog service and start it:

```
sudo systemctl enable timelog.service
sudo systemctl start timelog.service
```

## Usage

### Stopping, starting and restarting the server

Stopping the server:

```
sudo systemctl stop timelog.service
```

Starting the server:

```
sudo systemctl start timelog.service
```

Restarting the server:

```
sudo systemctl restart timelog.service
```

### Default Django site admin web interface

#### Adding users

Add regular users via `http://example.com:8000/admin/auth/user/add/`
(substitute your server host name or IP address for `example.com`).

#### Entering data

After you've added one or more regular users (see above), you *could* then
begin entering data into django-timelog right now at
`http://example.com:8000/admin/timelog/` (substitute your server host name
or IP address for `example.com`). It's not great but it's better than nothing.

### Timelog web UI

Browse to `http://example.com:8000/timelog/` (substitute your server host name
or IP address for `example.com`). It'll be blank unless you add a category
first. To add a category, use the above mentioned Django
site admin web interface.

### Dumping data for backup and later restore

```
sudo -u timelog -i -- bash -c \
  "cd ~/venv/serve/timelog/ \
   && python3 ~/venv/serve/manage.py dumpdata \
        > ~/timelog-\$( git describe )-dbdump-$( date +%FT%H%M%S%z ).json"
```

Transfer the data dump to somewhere safe!

## Updating and upgrading

Please give the update script a read-through after you've downloaded it
and prior to running it.

```
wget https://raw.githubusercontent.com/erikano/django-timelog/master/scripts/timelog-up.bash
sudo bash ./timelog-up.bash
```

## Copyright and license

```
Copyright (c) 2015 Erik Nordstroem <contact@erikano.net>
```

django-timelog is published under the ISC license.

A copy of the license is included in the file `LICENSE`.
