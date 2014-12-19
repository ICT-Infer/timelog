# timelog

Log and report time

## Usage

`tl begin <location> <tz> [-c <comment>] [<ts>]` -- Begin interval.

`tl end <id> <location> <tz> [-c <comment>] [<ts>]` -- End interval.

`tl report [<timestamp begin>] [<timestamp end>]` -- Show report.

`tl dump` -- Dump whole log to stdout. Useful if you ever want to migrate away.

## Timestamp (<ts>) format

Subset of ISO 8601; `<YYYY>-<MM>-<DD>T<HH>:<MM>:<ss>`.

### Example timestamp

`2014-12-19T20:23`

## Timezone (<tz>)

`/usr/share/zoneinfo/` on Debian systems.

### Example timezone

`Europe/Oslo`.

## Dependencies

`dbm.h`

## Compiling timelog

### FreeBSD

No instructions yet.

### Debian

```
sudo apt-get install gdbm-dev
make
```
