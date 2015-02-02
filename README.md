# tl (timelog)

Stack-oriented time tracking.

## Description

`tl` is a command-line utility for logging time and generating reports.

`tl` supports a set of seven commands; `tl init`, `tl timepoint`,
`tl pending`, `tl pop-drop`, `tl merge-add`, `tl unlog` and `tl report`.

### Timestamp format

The timestamp format of `tl timepoint` is
`[<YYYY>-<mm>-<dd>T]<HH>:<MM>`. (Similar to, but not quite, ISO 8601.)

### Time zone

Timestamps are not good for much without a time zone.

Time zone is included in the timestamp as provided by your OS.

The time zone recorded by `tl timepoint` will be used when presenting
timepoints (such as by `tl pending` and `tl report`).

### Known issues

* When a timepoint is deleted from the timepoint-stack (tps),
  the record is not removed, so the file doesn't shrink.
  This by itself wouldn't be too bad if it wasn't for the next point.
* When a timepoint is added to the tps, and the tps already has empty
  records so we are overwriting empty records to combat the above problem,
  the tps *keeps* growing, receiving an additional empty record at the end.
* The tests only check the return values of functions.
  They need to be rewritten so that they instead test the actual outcome.
* Some operating systems, when faced with an invalid `$TZ`, will
  silently ignore it and use UTC.

## Supported platforms

`tl` is being developed on FreeBSD/armv6, FreeBSD/amd64 and OpenBSD/i386.

## Compiling

```
make test
```
