#!/usr/bin/env sh

passed=0
failed=0

tests=sheet_json.py

for test in $tests; do
  (python3 ../../manage.py shell <<EOF
exec(open("$test").read())
EOF
  ) \
  && passed=`expr $passed + 1` \
  || failed=`expr $failed + 1`
done

echo "$passed/`expr $passed + $failed` passed, $failed failed." 1>&2

exit $failed
