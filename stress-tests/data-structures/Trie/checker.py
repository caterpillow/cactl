import subprocess

name = "Trie"

for i in range(1, 10000):
    if i < 10 or i % 100 == 0:
        print(i)
    subprocess.call("./gen > in", shell = True)
    r1 = subprocess.check_output(f"./{name} < in", shell = True).decode().strip()
    r2 = subprocess.check_output(f"./{name}brute < in", shell = True).decode().strip()
    if r1 != r2:
        print("\033[91mfound breaking after", i, "iteration(s)")
        print("\033[91mexpected:\033[0m")
        print(r2)
        print("\033[91mreceived\033[0m")
        print(r1)
        break
