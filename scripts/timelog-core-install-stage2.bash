#!/usr/bin/env bash

if [ ! "$( whoami )" == "timelog" ] ; then
  echo "This script needs to run as the user \`timelog'." 1>&2
  exit 1
fi

script_path=$( dirname $0 )

cd ${script_path}/../

python3 /usr/lib/python3/dist-packages/virtualenv.py --python=python3.4 ~/venv/
cd ~/venv/ && source bin/activate || exit 2

echo 'source ~/venv/bin/activate' >> ~/.bash_profile
pip3 install --upgrade pip
pip3 install -r requirements.txt

cd ~timelog/venv/
django-admin startproject serve
