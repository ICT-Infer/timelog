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

## Supported platforms

`tl` is being developed on:

  * FreeBSD/armv6
  * FreeBSD/amd64
  * OpenBSD/i386
  * Mac OS X/powerpc

## Known issues

### Ticketed

(Note that this is not meant to be a list of *all* ticketed issues, but rather,
to be a list of currently open tickets regarding issues considered to be
noteable pertaining to building and running saas-by-erik/timelog.)

* [#4](https://github.com/saas-by-erik/timelog/issues/4):
  The tests only check `tl` execution return values.
  They need to be rewritten so that they additionally test the actual outcome.
* [#13](https://github.com/saas-by-erik/timelog/issues/13):
  Unlog of entries e\_n..e\_m by repeatedly removing e\_n
  requires (m - n + 2) instead of (m - n + 1) runs.
* [#5](https://github.com/saas-by-erik/timelog/issues/5):
  Timelogs are tied to the endianness of the host they were generated on.
  Endianness to use when reading and writing timelog files should be chosen so
  that timelogs are "portable" between little-endian and big-endian hosts.
* [#9](https://github.com/saas-by-erik/timelog/issues/9):
  No install target currently exists in Makefile.
  It is common to have a target to be able to `make install`.

### Not ticketed

These are issues for which no tickets have been made since
it has not yet been determined whether tickets
should be created for these issues.

* Some operating systems, when faced with an invalid `$TZ`, will
  silently ignore it and use UTC.

## Building

```
./configure.sh && make test
```
