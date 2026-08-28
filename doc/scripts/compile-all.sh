#!/usr/bin/env bash
DIR=${1:-.}

# use a precompiled header for the template to improve perf
g++ -Wall -Wextra -Wfatal-errors -Wconversion -std=c++17 -x c++-header $DIR/content/contest/template.cpp
mkdir -p $DIR/build
SKIP=$DIR/build/skip_headers.txt
trap "rm -f $DIR/content/contest/template.cpp.gch $SKIP" EXIT

SCRIPT_DIR=$DIR/doc/scripts
# skip_headers: one path per line, '#' comments and blank lines allowed.
# Entries are matched as fixed substrings of the header path (grep -F),
# so they are written as full "content/<chapter>/<Name>.h" paths.
sed -e 's/#.*//' -e 's/[[:space:]]*$//' -e '/^$/d' $SCRIPT_DIR/skip_headers >$SKIP
tests="$(find $DIR/content -name '*.h' | grep -vFf $SKIP)"
echo "skipped: "
find $DIR/content -name '*.h' | grep -Ff $SKIP
declare -i pass=0
declare -i fail=0
failHeaders=""
for test in $tests; do
    echo "$(basename $test): "
    $SCRIPT_DIR/test-compiles.sh $test
    retCode=$?
    if (($retCode != 0)); then
        echo $retCode
        fail+=1
        failHeaders="$failHeaders$test\n"
    else
        pass+=1
    fi
    echo
done
echo "$pass/$(($pass+$fail)) tests passed"
if (($pass == 0)); then
    echo "No tests found (check skip_headers: an entry matching every path skips everything)"
    exit 1
elif (($fail == 0)); then
    echo "No tests failed"
    exit 0
else
    echo -e "These tests failed: \n $failHeaders"
    exit 1
fi
