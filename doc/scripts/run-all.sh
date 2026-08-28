#!/usr/bin/env bash
DIR=${1:-.}

# use a precompiled header for the template to improve perf
g++ -Wall -Wfatal-errors -Wconversion -std=c++17 -O2 $DIR/stress-tests/utilities/template.h
trap "rm -f $DIR/stress-tests/utilities/template.h.gch" EXIT

# Helper programs (generators, brute-force reference solutions, anything in a
# helpers/ subdirectory) are not self-checking tests and are not run.
helper=( '(' -path '*/helpers/*' -o -name 'gen.cpp' -o -iname '*brute.cpp' ')' )
echo "helpers (not run): "
find $DIR/stress-tests -name '*.cpp' "${helper[@]}" | sort
tests="$(find $DIR/stress-tests -name '*.cpp' -not "${helper[@]}" | sort)"
declare -i pass=0
declare -i fail=0
failTests=""
ulimit -s 524288 # For 2-sat test
for test in $tests; do
    echo "$(basename $test): "
    start=`date +%s.%N`
    # stdin is closed so a test that reads input fails instead of hanging
    g++ -Wall -Wfatal-errors -Wconversion -std=c++17 -O2 $test && ./a.out </dev/null
    retCode=$?
    if (($retCode != 0)); then
        echo "Failed with $retCode"
        fail+=1
        failTests="$failTests$test\n"
    else
        pass+=1
    fi
    end=`date +%s.%N`
    runtime=$( echo "$end - $start" | bc -l )
    echo "Took $runtime seconds"
    rm -f a.out
    echo
done
echo "$pass/$(($pass+$fail)) tests passed"
if (($fail == 0)); then
    echo "No tests failed"
    exit 0
else
    echo -e "These tests failed: \n $failTests"
    exit 1
fi
