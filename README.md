# atl - Alternate Timelog

Currently a work in progress.

## Setup

Describing the setup procedure using Debian GNU/Linux 7.

```
# apt-get install postgresql libpq-dev python3-pip
# pip-3.2 install django psycopg2
# adduser atl
# su - postgresql
$ psql
```

```
CREATE USER atl;
CREATE DATABASE atl OWNER atl;
\q
```

```
$ ^D
# su - atl
$ git clone https://github.com/erikano/atl.git
$ cd atl/
$ psql < atl.sql
```
