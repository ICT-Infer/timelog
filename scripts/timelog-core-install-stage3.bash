#!/usr/bin/env bash

if [ ! "$( whoami )" == "timelog" ] ; then
  echo "This script needs to run as the user \`timelog'." 1>&2
  exit 1
fi

script_path=$( dirname $0 )

cd ~timelog/venv/serve/

patch -p2 -d serve/ < ${script_path}/../patch/serve/settings.py.patch || exit 2
patch -p2 -d serve/ < ${script_path}/../patch/serve/urls.py.patch || exit 3
sed -i "s@\\\(TIME_ZONE = \\\)'[^']*'\\\$@\\\1'$( tzselect )'@" \
  serve/settings.py || exit 4
python3 manage.py makemigrations timelog || exit 5
python3 manage.py migrate || exit 6

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

python3 manage.py shell <<EOF || exit 7
from django.contrib.auth.models import User
User.objects.create_superuser('timelog', '', '$wui_pass')
EOF

python3 manage.py shell < timelog/scripts/setup/timelog_user_group.py || exit 8
