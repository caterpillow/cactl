#!/usr/bin/env python3
"""Per-line spacing normaliser for notebook headers. Never changes tokens or
line structure: indentation tabs->4 spaces, `FOR (i, a, b)`/`if (`, space
after commas and for-loop semicolons, spaces around binary operators
(templates' < > untouched), `(int) x` casts, `[&] (` lambdas, `) {`."""
import re, sys, os, subprocess, hashlib

OPS = r'(<<=|>>=|==|!=|<=|>=|&&|\|\||\+=|-=|\*=|/=|%=|&=|\|=|\^=|<<|>>|\+|-|\*|/|%|&|\||\^|=)'
BIN = re.compile(r'(?<!operator)(?<=[\w\]\)])' + OPS + r'(?=[\w\(\[~!])')
EXP = re.compile(r'\d[eE]$')  # 1e-9

def mask(code):
    lits = []
    def rep(m): lits.append(m.group(0)); return f'\x00{len(lits)-1}\x00'
    code = re.sub(r'/\*.*?\*/|"(\\.|[^"\\])*"|\'(\\.|[^\'\\])+\'', rep, code)
    return code, lits
def unmask(code, lits):
    return re.sub(r'\x00(\d+)\x00', lambda m: lits[int(m.group(1))], code)

def fix_code(c):
    c, lits = mask(c)
    c = re.sub(r'\b(FOR|F0R|ROF|if|for|while|switch)\(', r'\1 (', c)
    c = re.sub(r'\)\{', ') {', c); c = re.sub(r'\}else\b', '} else', c); c = re.sub(r'\belse\{', 'else {', c)
    c = re.sub(r',(?=[^\s\x00])', ', ', c)
    c = re.sub(r';(?=[^\s)])', '; ', c)
    c = re.sub(r'\[(&|=)?\]\(', r'[\1] (', c)
    c = re.sub(r'\((int|ll|db|ull|char|double|long double|size_t|unsigned|bool|float|ld)\)(?=[\w(\[~!*&-])', r'(\1) ', c)
    def binrep(m):
        s, e = m.start(), m.end()
        if EXP.search(c[:s]) and m.group(1) in '+-': return m.group(0)
        return ' ' + m.group(1) + ' '
    c = BIN.sub(binrep, c)
    c = re.sub(r'  +', ' ', c)  # collapse doubles we may have created
    # for (init; cond; step): < > in cond are comparisons, space them
    def forrep(m):
        parts = m.group(2).split(';')
        if len(parts) == 3:
            parts[1] = re.sub(r'(?<=[\w\]\)])(<=|>=|<|>)(?=[\w\(\[~!-])', r' \1 ', parts[1])
            parts[1] = re.sub(r'  +', ' ', parts[1])
        return m.group(1) + ';'.join(parts) + ')'
    c = re.sub(r'(\bfor \()([^()]*(?:\([^()]*\)[^()]*)*)\)', forrep, c)
    return unmask(c, lits)

def split_comment(line):
    # find first // not inside a string
    i, ins = 0, None
    while i < len(line):
        ch = line[i]
        if ins: 
            if ch == '\\': i += 1
            elif ch == ins: ins = None
        elif ch in '"\'': ins = ch
        elif line.startswith('//', i): return line[:i], line[i:]
        i += 1
    return line, ''

def process(text):
    out, inblock, changed = [], False, 0
    for line in text.split('\n'):
        s = line.strip()
        orig = line
        if inblock or s.startswith('/*') or s.startswith('*/') or s.startswith('* ') or s == '*':
            if s.startswith('/*'): inblock = True
            if '*/' in s: inblock = False
            out.append(line); continue
        if s.startswith('#') or s.startswith('//') or not s:
            out.append(line.rstrip()); continue
        ind = len(line) - len(line.lstrip())
        indent = line[:ind].replace('\t', '    ')
        code, com = split_comment(line[ind:].rstrip())
        code = fix_code(code.rstrip())
        new = indent + code + (' ' + com if com and code else com)
        if new != orig: changed += 1
        out.append(new)
    return '\n'.join(out), changed

def tokhash(text):
    return hashlib.md5(re.sub(r'\s+', '', text).encode()).hexdigest()

if __name__ == '__main__':
    apply = '--apply' in sys.argv
    files = [f for f in sys.argv[1:] if not f.startswith('--')]
    tot = 0
    for f in files:
        t = open(f).read(); n, ch = process(t)
        assert tokhash(t) == tokhash(n), f'TOKEN CHANGE in {f}'
        tot += ch
        if apply and n != t: open(f, 'w').write(n)
    print(f'{"applied" if apply else "dry-run"}: {tot} lines changed across {len(files)} files')
