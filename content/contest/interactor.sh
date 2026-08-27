#!/usr/bin/env bash
# Runs JUDGE and SOL with each one's stdout wired to the other's stdin.
# Usage: ./interactor.sh JUDGE SOL [input_file]
#   ./interactor.sh ./judge ./sol in
#   ./interactor.sh "python3 judge.py" ./sol      (quote multi-word commands)
# input_file is passed to JUDGE as argv[1]: ifstream fin(argv[1]); cin/cout
# stay the interaction. Both sides must flush after every write. A side that
# hangs is killed after T seconds (rc 124); the script exits nonzero if either
# side does, so in a stress loop: ./gen $i > in; ./interactor.sh J S in || break
T=${T:-1}
rm -f pipe; mkfifo pipe
timeout $T $1 $3 < pipe | timeout $T $2 > pipe
r=(${PIPESTATUS[@]}); rm -f pipe
echo "judge rc ${r[0]}, sol rc ${r[1]}"
((r[0] == 0 && r[1] == 0))
