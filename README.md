# django-timelog

Django Timelog App. Time sheet time tracking for a person or group of people.

## Table of Contents

* [Pre-stable notice](#pre-stable-notice)
* [Screenshot](#screenshot)
* [Security](#security)
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

## Pre-stable notice

Until 1.0.0 is reached, django-timelog is subject to change
in compatibility-breaking ways. Basically, in its
present stage of development, the only requirement is that
the following data MUST be preserved:

* User account name and password hash
* Category names and descriptions, as well as category hierarchy
* Entry begin and end dates, times and time zones; entry descriptions

Note that the names of the fields themselves may change
and the data representation may change. Anything else
not covered by the above requirement may change as well.

In other words; django-timelog is already ready for use
but keep in mind that if you're planning on doing
any integration work prior to release 1.0.0,
breakage may occur.

After 1.0.0, all releases up to but not including 2.0.0
will remain compatible in ways TBD.

This is (loosely?) following [semantic versioning](http://semver.org/).

## Screenshot

TODO: Add a screenshot.

## Security

django-timelog protects viewing and modifying data using the Django permissions system. The super user is able to use the default Django site admin web interface for additional operations. Regular users are able to view, add, change and delete categories and entries.

Regular users are trusted to operate on categories and entries regardless of ownership. In other words, some security is traded for convenience. This is useful, e.g. in the case where a group of people have been working on something together and they want one person to take care of entering the relevant data into timelog without having to muck about with permission systems.

TLS is currently not configured by default, meaning that all data, including usernames and passwords, are transmitted in the clear over the network. There are plans to set up TLS with self-signed certificates by default in a future release.

Security measures such as protections against CSRF, XSS and SQLi are provided by Django. See https://docs.djangoproject.com/en/1.8/topics/security/ for details.

## Supported platforms

* Debian GNU/Linux 8.0 "Jessie"

## Dependencies

Most notable dependencies:

  * Nginx
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
wget -N https://raw.githubusercontent.com/erikano/django-timelog/master/scripts/timelog-install.bash
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

1. Log in as *timelog* using the password you have set for that user.
2. Fill in username for the new user, password for the new user
   and password confirmation for the new user.
3. Click *save and continue editing*.
4. You are brought to the *change user* form.
5. In *available groups* under *groups*, select and choose *timelog_user*.
6. 
   * If you want to add more users, click *save and add another*.
     Repeat steps 2-6 until done adding all users you need.
   * If this was the last user you wanted to add, click *save*.
     Continue to step 7.
7. Log out from the Django site admin web interface and close the tab/window.

#### Entering data

After you've added one or more regular users (see above), you *could* then
begin entering data into django-timelog right now at
`http://example.com:8000/admin/timelog/` (substitute your server host name
or IP address for `example.com`). It's not great but it's better than nothing.

### Timelog web UI

Browse to `http://example.com:8000/timelog/`
(substitute your server host name or IP address for `example.com`).
Log in as [the user you created for yourself](#adding-users).

The page will be blank unless you add a category first.
To add a category, use the above mentioned Django site admin web interface.

### Dumping data for backup and later restore

```
sudo -u timelog -i -- bash -c \
  "python3 ~/venv/serve/manage.py dumpdata \
     > ~/timelog-$( cd ~timelog/venv/serve/timelog/ ; git describe )-dbdump-$( date +%FT%H%M%S%z ).json"
```

For good measure, we'll also do a dump of the whole database
using PostgreSQL tools. The reason for this is that
we shall prefer to restore using the Django tools
but if s--t gets f--ked, it's better to have
the additional means of recovery provided by this.

```
sudo -u timelog -i -- bash -c \
  "pg_dump timelog \
     > ~/timelog-$( cd ~timelog/venv/serve/timelog/ ; git describe )-dbdump-$( date +%FT%H%M%S%z ).sql"
```

Transfer the data dumps to somewhere safe!

## Updating and upgrading

Please give the update script a read-through after you've downloaded it
and prior to running it.

```
wget -N https://raw.githubusercontent.com/erikano/django-timelog/master/scripts/timelog-up.bash
sudo bash ./timelog-up.bash
```

## Copyright and license

django-timelog is copyright © 2015 Erik Nordstroem.

django-timelog is published under the ISC license.

A copy of the license is included in the file `LICENSE`.
