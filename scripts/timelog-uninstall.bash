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

sudo -u nobody bash $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/timelog-install.bash --version-check-only || exit 3

function uninstall_timelog {

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

  read -p "Really remove timelog service, user and files? [y/N] " -n 1 -r
  if [ ! $REPLY == "" ] ; then
    echo
  fi
  if [[ ! $REPLY =~ ^[Yy]$ ]]
  then
    echo "Abort." 1>&2
    exit 1
  fi

  systemctl disable timelog.service
  rm /etc/systemd/system/timelog.service
  systemctl daemon-reload
  rm /etc/nginx/sites-available/timelog
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

  echo "Done uninstalling timelog." 1>&2
  echo "Dependencies installed using apt-get" \
    "will have to be removed manually." 1>&2
}

uninstall_timelog
