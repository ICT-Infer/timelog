#!/usr/bin/env bash

export HOME=/var/lib/timelog
source ~/venv/bin/activate
python3 ~/venv/serve/manage.py runserver 0.0.0.0:8000
