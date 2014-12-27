# timelog

Log and report time

## Usage

`tl begin <location> [-c <comment>] [<ts>]` -- Begin interval.

`tl end <id> <location> [-c <comment>] [<ts>]` -- End interval.

`tl report [<ts> [<ts>]]` -- Show report.

## Timestamp (&lt;ts&gt;) format

Subset of ISO 8601; `<YYYY>-<MM>-<DD>T<HH>:<MM>:<ss>`.

### Example timestamp

`2014-12-19T20:23`

## Timezone

Retrieved from environment variable `TZ`.

### Example timezone

`Europe/Oslo`.

## Dependencies

`dbopen(3)` database access methods.

## Compiling timelog

### FreeBSD

No instructions yet.

### Debian

```
sudo apt-get install libdb-dev
make
```
