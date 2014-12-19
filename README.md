# timelog

Log and report time

## Usage

`tl begin <location> <tz> [-c <comment>] [<ts>]` -- Begin interval.

`tl end <id> <location> <tz> [-c <comment>] [<ts>]` -- End interval.

`tl report [<timestamp begin>] [<timestamp end>]` -- Show report.

`tl dump` -- Dump whole log to stdout. Useful if you ever want to migrate away.

## Timestamp (&lt;ts&gt;) format

Subset of ISO 8601; `<YYYY>-<MM>-<DD>T<HH>:<MM>:<ss>`.

### Example timestamp

`2014-12-19T20:23`

## Timezone (&lt;tz&gt;)

`/usr/share/zoneinfo/` on Debian systems.

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
