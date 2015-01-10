#!/usr/bin/env bash

n_tests=0
n_tests_failed=0
n_tests_passed=0

tempdir=$( mktemp -d )
if [ $? -ne 0 ] ; then
  echo "Failed to create temporary directory for tests." 1>&2
  exit 1
fi

origdir=$( pwd )
cd "$tempdir"

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl init 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl init'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -d .tl/ ] ; then
  echo "Test: Directory \`.tl/' created. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -f .tl/tl.db ] ; then
  echo "Test: File \`.tl/tl.db' created. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -f .tl/tps.db ] ; then
  echo "Test: File \`.tl/tps.db' created. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl init 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: \`tl init' where \`tl init' has been done. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: Run \`tl' without arguments. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl x 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: \`tl x' -- Invalid command. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl timepoint 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl timepoint'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl pending 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl pending'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl pop-drop 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl pop-drop'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl merge-add 2>/dev/null
if [ $? -eq 0 ] ; then
  echo -n "Test: \`tl merge-add' " 1>&2
  echo "with less than two points on stack. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl timepoint 2>/dev/null
$origdir/bin/tl timepoint 2>/dev/null
$origdir/bin/tl merge-add 2>/dev/null
if [ $? -ne 0 ] ; then
  echo -n "Test: \`tl merge-add' " 1>&2
  echo "with multiple points on stack. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl unlog 2 2>/dev/null
if [ $? -eq 0 ] ; then
  echo -n "Test: \`tl unlog' second log entry " 1>&2
  echo "with presumably only one entry in log. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl unlog 1 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl unlog' first log entry. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl unlog 1 2>/dev/null
if [ $? -eq 0 ] ; then
  echo -n "Test: \`tl unlog' " 1>&2
  echo "first log entry again. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl report >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl report'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rm .tl/tps.db 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: Remove \`.tl/tps.db'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rm .tl/tl.db 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: Remove \`.tl/tl.db'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rmdir .tl/ 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: Remove directory \`.tl/'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

cd "$origdir"
rmdir "$tempdir"
if [ $? -ne 0 ] ; then
  echo "Failed to remove temporary directory for tests." 1>&2
  exit 1
fi

echo "Ran $n_tests tests. $n_tests_passed passed. $n_tests_failed failed."

if [ $(( $n_tests_passed + $n_tests_failed )) -ne $n_tests ] ; then
  echo "Number of passed and failed mismatch total number of tests." 1>&2
  exit 1
fi

exit $n_tests_failed
