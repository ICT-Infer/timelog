#!/usr/bin/env bash

if [ "$( id -u )" -ne "0" ] ; then
  echo "Need root privileges to run." 1>&2
  exit 1
fi

script_path=$( dirname $0 )

if [ "$1" == "--with-avahi" ] ; then
  opt_avahi_service=true
fi

ver="0.3.8-git" # *Only* bump to milestone when changing either meta-package.

cd $script_path/../

dpkg-deb -b packaging/debian/meta/timelog-core-deps-${ver}/

dpkg -l gdebi-core 2>&1 | egrep -q "^ii  "
if [ "$?" -ne 0 ] ; then
  apt-get -y install gdebi-core
  script_installed_gdebi=true
fi

gdebi --n packaging/debian/meta/timelog-core-deps-${ver}.deb

if [ "$opt_avahi_service" == "true" ] ; then
  dpkg-deb -b packaging/debian/meta/timelog-core-extras-avahi-${ver}/
  gdebi --n packaging/debian/meta/timelog-core-extras-avahi-${ver}.deb
fi

if [ "$script_installed_gdebi" == "true" ] ; then
  apt-get -y remove gdebi-core
fi

# In case user is upgrading from a release prior to rename (i.e. pre-0.3.8).
dpkg -l django-timelog-base 2>&1 | egrep -q "^ii  "
if [ "$?" -eq 0 ] ; then
  apt-get -y remove django-timelog-base
fi
dpkg -l django-timelog-avahi 2>&1 | egrep -q "^ii  "
if [ "$?" -eq 0 ] ; then
  apt-get -y remove django-timelog-avahi
fi
if [ -d ~timelog/meta-packaging/ ] ; then
  rm -rf ~timelog/meta-packaging/django-timelog*
  rmdir ~timelog/meta-packaging/
fi
