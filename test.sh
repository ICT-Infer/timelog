#!/usr/bin/env bash

n_tests=0
n_tests_failed=0
n_tests_passed=0

export TLDATABASE=tl_test.db

if [ -f $TLDATABASE ] ; then
  echo "tl db file $TLDATABASE exists. Cannot run tests." 1>&2
  exit 1
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Argument count test failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl x 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Command name test failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ -f "$TLDATABASE" ] ; then
  rm "$TLDATABASE"
  if [ $? -ne 0 ] ; then
    n_tests_failed=$(( $n_tests_failed + 1 ))
  else
    n_tests_passed=$(( $n_tests_passed + 1 ))
  fi
else
  echo 'Test for existence of tl database failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
fi

echo "Ran $n_tests tests. $n_tests_passed passed. $n_tests_failed failed."

if [ $n_tests_passed -lt $n_tests ] ; then
  exit 1
fi
