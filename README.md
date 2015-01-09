# tl (timelog)

Stack-oriented time tracking.

## Description

`tl` is a command-line utility for logging time and generating reports.

`tl` supports a set of seven commands; `tl init`, `tl timepoint`,
`tl pending`, `tl pop-drop`, `tl merge-add`, `tl unlog` and `tl report`.

### Timestamp format

The timestamp format of `tl timepoint` is
`[<YYYY>-<mm>-<dd>]T<HH>:<MM>[:<SS>]`. (Similar to, but not quite, ISO 8601.)

### Time zone

Timestamps are not good for much without a time zone.

The `TZ` environment variable, if set and not empty,
will be used by `tl timepoint`.
If the `TZ` environment variable is not set or is empty,
`tl timepoint` will use the system time zone.

The time zone recorded by `tl timepoint` will be used when presenting
timepoints (such as by `tl pending` and `tl report`).

## Supported platforms

`tl` is being developed on FreeBSD/armv6 and OpenBSD/i386.

## Compiling

```
make test
```
