#!/usr/bin/env bash

export HOME=/var/lib/timelog
source ~/venv/bin/activate
uwsgi /var/lib/timelog/venv/serve/timelog/uwsgi-config/timelog.ini
