# tl (timelog)

Time logging and reporting using stacks and stuff :D

## Description

`tl` is a command-line utility for logging time and generating reports.

`tl` supports a set of seven commands; `tl init`, `tl push-point`,
`tl cheat-dump-stack`, `tl pop-drop-point`, `tl pop-twice-merge-points-log`,
`tl unlog-split-push-points` and `tl show-report`. Those are the descriptive
names. Shorter aliases might be a good idea. Your shell probably supports
aliasing commands. Right?

Please refer to the source code for further details beyond here noted ;)

### Timestamp format

The timestamp format of `tl push-point` is
`[<YYYY>-<mm>-<dd>]T<HH>:<MM>[:<SS>]`. (Similar to, but not quite, ISO 8601.)

For more flexible input, pass your input through `date` like so:

```
tl push-point -l "Home at Nesodden" -m "Resumed work with tl (how meta)." \
  -t "$( date +'%Y-%m-%dT%H:%M:%S' --date='18:00 yesterday' )"
```

(Just be careful with time zones when passing time through `date` in this way.)

### Time zone

Timestamps are not good for much without a time zone.

The `TZ` environment variable, if set and not empty,
will be used by `tl push-point`.
If the `TZ` environment variable is not set, `tl push-point` will use
the system time zone.

The time zone recorded by `tl push-point` will be used when presenting
entries from the log.

## Supported platforms

`tl` is being developed on OpenBSD/i386.

## Compiling

```
make test
```
