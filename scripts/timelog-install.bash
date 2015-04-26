#!/usr/bin/env bash

if [ "$( id -u )" -ne "0" ] ; then
  echo "Need root privileges to run." 1>&2
  exit 1
fi

if [ ! "$( cat /etc/issue | head -n1 | cut -d' ' -f1-3 )" == "Debian GNU/Linux 8" ] ; then
  echo "This install script is made for Debian GNU/Linux 8." 1>&2
  echo "You do not appear to be running Debian GNU/Linux 8." 1>&2
  exit 2
fi

apt-get install postgresql libpq-dev python3-pip

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
   && wget \
        https://labix.org/download/python-dateutil/python-dateutil-2.0.tar.gz \
   && tar xvf python-dateutil-2.0.tar.gz \
   && cd python-dateutil-2.0/ \
   && python3 setup.py install \
   && cd ../ \
   && pip3 install django pytz Unidecode Jinja2 psycopg2 \
   && django-admin startproject serve \
   && cd serve/ \
   && git clone https://github.com/erikano/django-timelog.git timelog/ \
   && patch -p2 -d serve/ < timelog/patch/serve/settings.py.patch \
   && patch -p2 -d serve/ < timelog/patch/serve/urls.py.patch \
   && sed -i \"s@\\\(TIME_ZONE = \\\)'[^']*'\\\$@\\\1'$( tzselect )'@\" \
        serve/settings.py \
   && python3 manage.py makemigrations timelog \
   && python3 manage.py migrate \
   && python3 manage.py createsuperuser"

cp /var/lib/timelog/venv/serve/timelog/systemd-service/timelog.service /etc/systemd/system/ && systemctl daemon-reload
