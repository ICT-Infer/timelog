#!/usr/bin/env bash

if [ "$#" -ne "0" ] ; then
  echo "Usage: $0" 1>&2
  exit 1
fi

if [ "$( id -u )" -ne "0" ] ; then
  echo "Need root privileges to run." 1>&2
  exit 2
fi

function abort_upgrade {

  sudo -u timelog -i -- bash -c \
    "cd ~/venv/serve/timelog \
     && git reset scripts/timelog-install.bash \
     && git checkout -- scripts/timelog-install.bash"

  exit 3
}

function upgrade_timelog {

  sudo -u timelog -i -- bash -c \
    "cd ~/venv/serve/timelog \
     && git fetch \
     && git checkout origin/master -- scripts/timelog-install.bash"

  sudo -u nobody bash ~timelog/venv/serve/timelog/scripts/timelog-install.bash --version-check-only || abort_upgrade || exit 3

  sudo -u timelog -i -- bash -c \
    "cd ~/venv/serve/timelog \
     && git reset scripts/timelog-install.bash \
     && git checkout -- scripts/timelog-install.bash \
     && git pull \
     && pip install -U -r requirements.txt \
     && python3 ../manage.py makemigrations timelog \
     && python3 ../manage.py migrate"

  sudo -u timelog -i -- bash -c \
    "python3 ~/venv/serve/manage.py shell \
      < ~/venv/serve/timelog/scripts/setup/timelog_user_group.py"

  echo "Restarting timelog service. This might take a while." 1>&2

  systemctl restart timelog.service

  echo "Done upgrading timelog." 1>&2

  sudo -u timelog -i bash -c \
    "cd ~/venv/serve/timelog \
     && git describe"
}

upgrade_timelog
