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

dpkg -l gdebi-core 1>&2 2>/dev/null
if [ "$?" -ne 0 ] ; then
  apt-get -y install gdebi-core
  script_installed_gdebi=true
fi

gdebi --n packaging/debian/meta/timelog-core-deps-${ver}.deb

# We also have gdebi-core as a dep but in the case where we
# first had to install it from this script itself, we now remove it
# so the packaging system will know that it is only installed as a
# dependency. Useful in case we decide to stop using it in the future
# and also it's the nice thing to do anyway.
if [ "$script_installed_gdebi" == "true" ] ; then
  apt-get -y remove gdebi-core
fi

if [ "$opt_avahi_service" == "true" ] ; then
  dpkg-deb -b packaging/debian/meta/timelog-core-extras-avahi-${ver}/
  gdebi --n packaging/debian/meta/timelog-core-extras-avahi-${ver}.deb
fi

# In case user is upgrading from a release prior to rename (i.e. pre-0.3.8).
dpkg -l django-timelog-base 2>&1 >/dev/null
if [ "$?" -eq 0 ] ; then
  apt-get -y remove django-timelog-base
fi
dpkg -l django-timelog-avahi 2>&1 >/dev/null
if [ "$?" -eq 0 ] ; then
  apt-get -y remove django-timelog-avahi
fi
if [ -d ~timelog/meta-packaging/ ] ; then
  rm -rf ~timelog/meta-packaging/django-timelog*
  rmdir ~timelog/meta-packaging/
fi
