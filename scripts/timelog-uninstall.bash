#!/usr/bin/env bash

if [ "$#" -eq "1" ] && [ "$1" == "--drop" ] ; then
  drop=true
elif [ "$#" -ne "0" ] ; then
  echo "Usage: $0 [--drop]" 1>&2
  exit 1
else
  unset drop
fi

if [ "$( id -u )" -ne "0" ] ; then
  echo "Need root privileges to run." 1>&2
  exit 2
fi

if [ ! "$( cat /etc/issue | head -n1 | cut -d' ' -f1-3 )" == "Debian GNU/Linux 8" ] ; then
  echo "This uninstall script is made for Debian GNU/Linux 8." 1>&2
  echo "You do not appear to be running Debian GNU/Linux 8." 1>&2
  exit 3
fi

read -p "Stop timelog service and kill all timelog user owned processes? [y/N] " -n 1 -r
if [ ! $REPLY == "" ] ; then
  echo
fi
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
  echo "Abort." 1>&2
  exit 1
fi

systemctl stop timelog.service
pkill -u timelog

read -p "Really remove timelog service, user and /var/lib/timelog/? [y/N] " -n 1 -r
if [ ! $REPLY == "" ] ; then
  echo
fi
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
  echo "Abort." 1>&2
  exit 1
fi

rm /etc/avahi/services/timelog.service
systemctl disable timelog.service
rm /etc/systemd/system/timelog.service
systemctl daemon-reload
userdel timelog
rm -rf /var/lib/timelog

if [ "$drop" == "true" ] ; then
  read -p "Really drop all timelog data from DB? [y/N] " -n 1 -r
  if [ ! $REPLY == "" ] ; then
    echo
  fi
  if [[ $REPLY =~ ^[Yy]$ ]]
  then
    sudo -u postgres -i psql <<EOF
DROP DATABASE timelog;
DROP USER timelog;
EOF
  else
    echo "Abort." 1>&2
    exit 1
  fi
fi
