# tl

Time logging and reporting

## Usage

`tl create [-f <file.db>]` -- Create time log.

`tl begin <location> [-c <comment>] [<ts>] [-f <file.db>]` -- Begin interval.

`tl end <id> <location> [-c <comment>] [<ts>] [-f <file.db>]` -- End interval.

`tl report [<ts> [<ts>]] [-f <file.db>]` -- Show report.

## Timestamp (&lt;ts&gt;) format

Similar to, but not quite, ISO 8601; `[<YYYY>-<MM>-<DD>]T<HH>:<MM>[:<ss>]`.

### Example timestamp

`2014-12-19T20:23`.

## Environment variables

`TZ` -- Time zone.

### Example time zone

`Europe/Oslo`.

## Supported platforms

`tl` is currently being developed on OpenBSD 5.4 i386.

## Compiling

```
make test
```
