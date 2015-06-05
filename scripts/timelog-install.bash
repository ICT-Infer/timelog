#!/usr/bin/env bash

if [ "$#" -eq "1" ] && [ "$1" == "--version-check-only" ] ; then
  version_check_only=true
elif [ "$#" -ne "0" ] ; then
  echo "Usage: $0 [--check]" 1>&2
  exit 1
else
  unset version_check_only
fi

if [ ! "$( cat /etc/issue | head -n1 | cut -d' ' -f1-3 )" == "Debian GNU/Linux 8" ] ; then
  echo "Only Debian GNU/Linux 8 is supported by this version of timelog." 1>&2
  echo "You do not appear to be running Debian GNU/Linux 8." 1>&2
  exit 2
fi

if [ "$version_check_only" == "true" ] ; then
  exit 0
fi

if [ "$( id -u )" -ne "0" ] ; then
  echo "Need root privileges to run." 1>&2
  exit 3
fi

function install_timelog {

  apt-get -y install gdebi-core
  mkdir -p /var/tmp/meta-packaging/django-timelog-base-0.2.3-git/DEBIAN/

  cat > /var/tmp/meta-packaging/django-timelog-base-0.2.3-git/DEBIAN/control <<EOF
Package: django-timelog-base
Version: 0.2.3-git
Section: main
Priority: standard
Architecture: all
Depends: git, build-essential, python3-dev, postgresql, libpq-dev, python3-pip, nginx
Maintainer: Erik Nordstroem <contact@erikano.net>
Description: Meta-package for dependencies of base django-timelog.
EOF

  dpkg-deb -b /var/tmp/meta-packaging/django-timelog-base-0.2.3-git/
  gdebi /var/tmp/meta-packaging/django-timelog-base-0.2.3-git.deb

  pip3 install virtualenv

  adduser --system --home /var/lib/timelog --group --shell /bin/bash timelog

  sudo -u postgres -i psql <<EOF
CREATE USER timelog;
CREATE DATABASE timelog OWNER timelog;
EOF

  sudo -u timelog -i -- bash -c \
    "virtualenv-3.4 ~/venv/ \
     && cd ~/venv/ \
     && source bin/activate \
     && echo 'source ~/venv/bin/activate' >> ~/.bash_profile \
     && git clone https://github.com/erikano/django-timelog.git timelog/ \
     && pip3 install --upgrade pip \
     && pip3 install -r timelog/requirements.txt \
     && django-admin startproject serve \
     && mv timelog/ serve/timelog/ \
     && cd serve/ \
     && patch -p2 -d serve/ < timelog/patch/serve/settings.py.patch \
     && patch -p2 -d serve/ < timelog/patch/serve/urls.py.patch \
     && sed -i \"s@\\\(TIME_ZONE = \\\)'[^']*'\\\$@\\\1'$( tzselect )'@\" \
          serve/settings.py \
     && python3 manage.py makemigrations timelog \
     && python3 manage.py migrate"

  echo
  echo "Django site admin web interface superuser account creation" 1>&2
  wui_user=timelog
  echo "Using username \`$wui_user'"
  read -p "Use a random password? [y/N] " -n 1 -r wui_pass_random
  if [ ! $wui_pass_random == "" ] ; then
    echo
  fi
  if [[ $wui_pass_random =~ ^[Yy]$ ]] ; then
    wui_pass=$( egrep -o ^[a-z]+$ /usr/share/dict/words | shuf -n4 | xargs echo )
    echo "Using random password \`$wui_pass'."
  else
    while [ -z $wui_pass ] || [ ! "$wui_pass_a" == "$wui_pass_b" ] ; do
      read -p "Enter password: " -r -s wui_pass_a
      echo
      read -p "Confirm password: " -r -s wui_pass_b
      echo
      wui_pass="$wui_pass_a"
    done
  fi

  sudo -u timelog -i -- bash -c \
    "python3 ~/venv/serve/manage.py shell" <<EOF
from django.contrib.auth.models import User
User.objects.create_superuser('timelog', '', '$wui_pass')
EOF

  sudo -u timelog -i -- bash -c \
    "python3 ~/venv/serve/manage.py shell" \
      < ~timelog/venv/serve/timelog/scripts/setup/timelog_user_group.py

  ln -s /var/lib/timelog/venv/serve/timelog/nginx-site/timelog \
    /etc/nginx/sites-available/

  echo "Zeroconf mDNS with Avahi (optional)"
  read -p "Install /etc/avahi/services/timelog.service? [y/N] " -n 1 -r opt_avahi_service
  if [ ! $opt_avahi_service == "" ] ; then
    echo
  fi
  if [[ $opt_avahi_service =~ ^[Yy]$ ]] ; then
    mkdir -p /var/tmp/meta-packaging/django-timelog-avahi-0.2.3-git/DEBIAN/

    cat > /var/tmp/meta-packaging/django-timelog-avahi-0.2.3-git/DEBIAN/control <<EOF
Package: django-timelog-avahi
Version: 0.2.3-git
Section: main
Priority: standard
Architecture: all
Depends: avahi-daemon
Maintainer: Erik Nordstroem <contact@erikano.net>
Description: Meta-package for dependencies of django-timelog Avahi integration.
EOF

    dpkg-deb -b /var/tmp/meta-packaging/django-timelog-avahi-0.2.3-git/
    gdebi /var/tmp/meta-packaging/django-timelog-avahi-0.2.3-git.deb

    cp /var/lib/timelog/venv/serve/timelog/systemd-service/timelog-a.service \
      /etc/systemd/system/timelog.service
  else
    cp /var/lib/timelog/venv/serve/timelog/systemd-service/timelog.service \
      /etc/systemd/system/timelog.service
  fi

  mkdir ~timelog/meta-packaging/
  mv /var/tmp/meta-packaging/django-timelog-*-0.2.3-git* \
    ~timelog/meta-packaging/
  chown -R timelog:timelog ~timelog/meta-packaging/
  rmdir /var/tmp/meta-packaging/

  systemctl daemon-reload

  echo "Done installing timelog." 1>&2
  echo "Remember to enable and start the timelog service." 1>&2
}

install_timelog
