"""
 * Author: caterpillow
 * Date: 2026-03-03
 * Description: Small program that runs two processes, connecting the stdin of each one to the stdout of the other.
"""

# Usage: python3 interactor.py ./judge ./sol [input_file]
# `input_file` is passed to judge as argv[1].
# int main(int argc, char** argv){
#    ifstream fin(argv[1]);
#    int n; fin >> n;
#    ...
# }
# In C++ judge: `ifstream fin(argv[1]);` read test data from `fin`; keep `cin/cout` for interaction.
# Contract: judge/solution must flush after each write; when one exits, the other is terminated.
import os, subprocess as sp, sys, threading as th, time
a1, a2, *a3 = sys.argv[1:]
j = sp.Popen([a1, *a3], stdin=sp.PIPE, stdout=sp.PIPE)
s = sp.Popen([a2], stdin=sp.PIPE, stdout=sp.PIPE)

def p(a, b):
    try:
        while d := os.read(a.fileno(), 1 << 16):
            os.write(b.fileno(), d)
        b.close()
    except OSError:
        pass

for a, b in ((s.stdout, j.stdin), (j.stdout, s.stdin)):
    th.Thread(target=p, args=(a, b), daemon=1).start()

while j.poll() is None and s.poll() is None:
    time.sleep(0.01)
for x in (j, s):  # grace period, then kill the survivor
    try: x.wait(1)
    except sp.TimeoutExpired: x.terminate()
print("Judge RC:", j.wait())
print("Sol RC:", s.wait())
