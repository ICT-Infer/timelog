# tl

Time logging and reporting

## Usage

`tl create [-f <file.db>]` -- Create time log.

`tl begin <location> [-c <comment>] [<ts>] [-f <file.db>]` -- Begin interval.

`tl end <id> <location> [-c <comment>] [<ts>] [-f <file.db>]` -- End interval.

`tl report [<ts> [<ts>]] [-f <file.db>]` -- Show report.

## Timestamp (&lt;ts&gt;) format

Subset of ISO 8601; `<YYYY>-<MM>-<DD>T<HH>:<MM>:<ss>`.

### Example timestamp

`2014-12-19T20:23`

## Environment variables

`TZ` -- Time zone.

### Example time zone

`Europe/Oslo`.

## Dependencies

`dbopen(3)` database access methods.

## Compiling

### FreeBSD

No instructions yet.

### Debian

```
sudo apt-get install libdb-dev
make
```

## Running tests

```
make test
```
