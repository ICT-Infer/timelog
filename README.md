# tl (timelog)

Stack-oriented time tracking.

## Description

`tl` is a command-line utility for logging time and generating reports.

`tl` supports a set of seven commands; `tl init`, `tl push-point`,
`tl cheat-dump-stack`, `tl pop-drop-point`, `tl pop-twice-merge-points-log`,
`tl unlog-split-push-points` and `tl show-report`. Those are the descriptive
names. Shorter aliases might be a good idea. Your shell probably supports
aliasing commands.

### Timestamp format

The timestamp format of `tl push-point` is
`[<YYYY>-<mm>-<dd>]T<HH>:<MM>[:<SS>]`. (Similar to, but not quite, ISO 8601.)

### Time zone

Timestamps are not good for much without a time zone.

The `TZ` environment variable, if set and not empty,
will be used by `tl push-point`.
If the `TZ` environment variable is not set or is empty,
`tl push-point` will use the system time zone.

The time zone recorded by `tl push-point` will be used when presenting
entries from the log.

## Supported platforms

`tl` is being developed on FreeBSD/armv6 and OpenBSD/i386.

## Compiling

```
make test
```
