#!/usr/bin/env bash

if [ ! "$( whoami )" == "timelog" ] ; then
  echo "This script needs to run as the user \`timelog'." 1>&2
  exit 1
fi

script_path=$( dirname $0 )

python3 /usr/lib/python3/dist-packages/virtualenv.py --python=python3.4 ~/venv/
cd ~/venv/ && source bin/activate || exit 2

echo 'source ~/venv/bin/activate' >> ~/.bash_profile
pip3 install --upgrade pip || exit 3
pip3 install -r ${script_path}/../requirements.txt || exit 4

cd ~timelog/venv/
django-admin startproject serve || exit 5
