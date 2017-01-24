#!/usr/bin/env bash

if [ "$#" -eq "1" ] && [ "$1" == "--version-check-only" ] ; then
  version_check_only=true
elif [ "$#" -ne "0" ] ; then
  echo "Usage: $0 [--version-check-only]" 1>&2
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

locale 2>&1 | grep -q "No such file or directory"
if [ "$?" -eq "0" ] ; then
  echo "You've got locale problems." 1>&2
  echo "Typically fixed with \`dpkg-reconfigure locales'" 1>&2
  exit 4
fi

which sudo >/dev/null
if [ "$?" -ne "0" ] ; then
  echo "Need \`sudo'. Please \`apt-get install sudo'." 1>&2
  exit 5
fi

# If you've modified timelog-core to use a remote database server,
# then this check won't work. It would be up to you to fix this check
# if you change timelog-core that way.
echo '\q' | sudo -u postgres psql timelog >/dev/null 2>&1
if [ "$?" -eq "0" ] ; then
  echo "Database \`timelog' exists." 1>&2
  exit 6
fi

id timelog >/dev/null 2>&1
if [ "$?" -eq "0" ] ; then
  echo "Unix user \`timelog' exists." 1>&2
  exit 7
fi

function install_timelog {
  tzsel=$( tzselect ) || exit 8

  echo "Django superuser account configuration" 1>&2
  wui_user=timelog
  echo "Using username \`$wui_user'"
  read -p "Use a random password? [y/N] " -n 1 -r wui_pass_random
  if [ ! $wui_pass_random == "" ] ; then
    echo
  fi
  if [[ $wui_pass_random =~ ^[Yy]$ ]] ; then
    wui_pass_random=true
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

  echo "Zeroconf mDNS with Avahi (optional)"
  read -p "Install /etc/avahi/services/timelog.service? [y/N] " -n 1 -r opt_avahi_service
  if [ ! "$opt_avahi_service" == "" ] ; then
    echo
  fi
  if [[ $opt_avahi_service =~ ^[Yy]$ ]] ; then
    opt_avahi_service=true
  fi

  dpkg -l git 2>&1 | egrep -q "^ii  "
  if [ "$?" -ne 0 ] ; then
    apt-get -y install git
    script_installed_git=true
  fi

  adduser --system --home /var/lib/timelog --group --shell /bin/bash timelog

  sudo -u timelog -i bash -c "git clone -b legacy-support https://github.com/ICT-Infer/timelog.git"

  if [ "$script_installed_git" == "true" ] ; then
    apt-get -y remove git
  fi

  if [ "$opt_avahi_service" == "true" ] ; then
    ~timelog/timelog/scripts/timelog-core-install-deps.bash --with-avahi \
    || exit 9
  else
    ~timelog/timelog/scripts/timelog-core-install-deps.bash || exit 10
  fi

  sudo -u postgres -i psql <<EOF
CREATE USER timelog;
CREATE DATABASE timelog OWNER timelog;
EOF

  sudo -u timelog -i bash \
    ~timelog/timelog/scripts/timelog-core-install-stage2.bash || exit 11

  mv ~timelog/timelog ~timelog/venv/serve/timelog

  echo "$wui_pass" \
  | sudo tzsel="$tzsel" wui_user="$wui_user" -u timelog -i bash \
      ~timelog/venv/serve/timelog/scripts/timelog-core-install-stage3.bash \
  || exit 12

  ln -s /var/lib/timelog/venv/serve/timelog/nginx-site/timelog \
    /etc/nginx/sites-available/ || exit 13

  if [[ $opt_avahi_service =~ ^[Yy]$ ]] ; then
    cp /var/lib/timelog/venv/serve/timelog/systemd-service/timelog-a.service \
      /etc/systemd/system/timelog.service || exit 14
  else
    cp /var/lib/timelog/venv/serve/timelog/systemd-service/timelog.service \
      /etc/systemd/system/timelog.service || exit 15
  fi

  systemctl daemon-reload

  echo "Done installing timelog." 1>&2
  echo "Django superuser account details:" 1>&2
  echo -n "  username \`" 1>&2
  echo -n "$wui_user" # echoed to stdout
  echo "'" 1>&2
  if [ "$wui_pass_random" == "true" ] ; then
    echo -n "  random password \`" 1>&2
    echo -n "$wui_pass" # echoed to stdout
    echo "'" 1>&2
  else
    echo -n "(user-entered password not shown)" 1>&2
    echo # blank line to stdout
  fi
  echo "Remember to enable and start the timelog service." 1>&2
}

install_timelog
