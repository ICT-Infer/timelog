#!/usr/bin/env bash

if [ ! "$( whoami )" == "timelog" ] ; then
  echo "This script needs to run as the user \`timelog'." 1>&2
  exit 1
fi

script_path=$( dirname $0 )

read -r -s wui_pass

cd ~timelog/venv/serve/

patch -p2 -d serve/ < ${script_path}/../patch/serve/settings.py.patch || exit 2
patch -p2 -d serve/ < ${script_path}/../patch/serve/urls.py.patch || exit 3
sed -i "s@\\\(TIME_ZONE = \\\)'[^']*'\\\$@\\\1'${tzsel}'@" serve/settings.py
grep -q "TIME_ZONE = $tzsel" serve/settings.py || exit 4
python3 manage.py makemigrations timelog || exit 5
python3 manage.py migrate || exit 6

python3 manage.py shell <<EOF || exit 7
from django.contrib.auth.models import User
User.objects.create_superuser('timelog', '', '$wui_pass')
EOF

python3 manage.py shell < timelog/scripts/setup/timelog_user_group.py || exit 8
