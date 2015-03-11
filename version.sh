#!/usr/bin/env sh

repo_version=$( git describe )
verstr=${repo_version:-0.b.c-NaN-gbadf00d}
vermajor=${verstr%%.*}
tmp=${verstr#*.}
verminor=${tmp%%.*}
tmp=${verstr##*.}
verminuscle=${tmp%%-*}

repo_branchname=$( git rev-parse --abbrev-ref HEAD )
branchname=${repo_branchname:-unknown}

git diff-index --quiet --cached HEAD
if [ $? -ne 0 ] ; then repo_staged=c ; fi
git diff-files --quiet
if [ $? -ne 0 ] ; then repo_unstaged=f ; fi
git diff-index --quiet HEAD
if [ $? -ne 0 ] ; then repo_stagecombo=i ; fi
test -z "$( git ls-files --others --exclude-standard )"
if [ $? -ne 0 ] ; then repo_untracked=o ; fi
repo_status="${repo_staged}${repo_unstaged}${repo_stagecombo}${repo_untracked}"
test -z "${repo_status}"
if [ $? -ne 0 ] ; then status=" +${repo_status}" ; fi

# TODO: Major, minor, minuscle
cat >_version.h <<EOF
#define TIMELOGVERSIONSTRING "${verstr} (${branchname}${status})"
#define TIMELOGVERSIONMAJOR ${vermajor}
#define TIMELOGVERSIONMINOR ${verminor}
#define TIMELOGVERSIONMINUSCLE ${verminuscle}
EOF

diff version.h _version.h 2>&1 >/dev/null \
  && rm _version.h \
  || mv _version.h version.h
