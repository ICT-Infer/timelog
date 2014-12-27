#!/usr/bin/env bash

n_tests=0
n_tests_failed=0
n_tests_passed=0

TLDATABASE="tl_test.db"

if [ -f "$TLDATABASE" ] ; then
  echo "tl db file $TLDATABASE exists. Cannot run tests." 1>&2
  exit 1
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl create -f "$TLDATABASE" 2>/dev/null
if [ $? -ne 0 ] ; then
  echo 'Test: Create named tl database. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -f "$TLDATABASE" ] ; then
  echo 'Test: Created named tl database file. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Test: No command provided #1. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl -f "$TLDATABASE" 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Test: No command provided #2. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl x 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Test: Invalid command #1. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
./bin/tl x -f "$TLDATABASE" 2>/dev/null
if [ $? -eq 0 ] ; then
  echo 'Test: Invalid command #2. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rm "$TLDATABASE" 2>/dev/null
if [ $? -ne 0 ] ; then
  echo 'Test: Remove tl database file. Failed.' 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

echo "Ran $n_tests tests. $n_tests_passed passed. $n_tests_failed failed."

if [ $n_tests_passed -lt $n_tests ] ; then
  exit 1
fi
