#!/usr/bin/env python3
# Source code preprocessor for the KACTL build process. License: CC0
#
# kactlpkg.sty runs it through \write18 from the repo root, once per template
# (`-i <file> -o build/kactl.tmp [-l lang]`: the file becomes a LaTeX fragment
# that \kactlimport \inputs) and once per page (`--print-header <mark>`: the
# list of templates starting on that page, state kept in ./header.tmp).
# Every failure must be loud: anything that goes wrong after option parsing
# is written to the output file as \kactlerror{...}, which stops pdflatex.

import getopt
import os
import re
import shutil
import subprocess
import sys
import traceback

HASH_SCRIPT = 'content/contest/hash.sh'   # printed in the notebook; typed on the judge
HEADER_STATE = 'header.tmp'

KNOWN_COMMANDS = ['Author', 'Date', 'Description', 'Source', 'Time', 'Memory',
                  'License', 'Status', 'Usage', 'Details']
REQUIRED_COMMANDS = ['Author', 'Description']


class PreprocessorError(Exception):
    pass


def escape(text):
    text = text.replace('<', r'\ensuremath{<}')
    text = text.replace('>', r'\ensuremath{>}')
    return text

def pathescape(text):
    text = text.replace('\\', r'\\')
    text = text.replace('_', r'\_')
    return escape(text)

def codeescape(text):
    text = text.replace('_', r'\_')
    text = text.replace('\n', '\\\\\n')
    text = text.replace('{', r'\{')
    text = text.replace('}', r'\}')
    text = text.replace('^', r'\ensuremath{\hat{\;}}')
    return escape(text)

def ordoescape(text, esc=True):
    if esc:
        text = escape(text)
    start = text.find("O(")
    if start >= 0:
        bracketcount = 1
        end = start + 1
        while end + 1 < len(text) and bracketcount > 0:
            end = end + 1
            if text[end] == '(':
                bracketcount = bracketcount + 1
            elif text[end] == ')':
                bracketcount = bracketcount - 1
        if bracketcount == 0:
            return r"%s\bigo{%s}%s" % (text[:start], text[start + 2:end], ordoescape(text[end + 1:], False))
    return text

def texsafe(text):
    """Error-message text: nothing that could confuse TeX inside \\kactlerror{}."""
    return re.sub(r'[\\{}#%$&_^~\n]', '?', text)


def runhash(text):
    """The 6-char hash hash.sh prints for `text`. cpp warnings are noise and are
    dropped; a missing cpp (md5 of empty input, d41d8c) or a failing pipeline
    is an error -- `sh` would return only the last command's status."""
    if shutil.which('cpp') is None:
        raise PreprocessorError("cpp not found (%s needs gcc's preprocessor)" % HASH_SCRIPT)
    p = subprocess.run(['bash', '-o', 'pipefail', HASH_SCRIPT], input=text,
                       capture_output=True, text=True)
    hsh = p.stdout.strip()
    if p.returncode != 0 or not re.fullmatch(r'[0-9a-f]{6}', hsh):
        detail = p.stderr.strip().splitlines()
        raise PreprocessorError("%s failed (exit %d)%s" % (
            HASH_SCRIPT, p.returncode, ": " + detail[-1] if detail else ""))
    return hsh

def hashmarkers(source):
    """Prefix-hash markers: a "<hash>" is replaced with the hash of the listing
    up to and including its line (the marker line itself is a comment, so it
    does not affect the hash)."""
    if '<hash>' not in source:
        return source
    hlines = source.split('\n')
    for i, hline in enumerate(hlines):
        if '<hash>' in hline:
            hlines[i] = hline.replace('<hash>', runhash('\n'.join(hlines[:i + 1])))
    return '\n'.join(hlines)

def tabify(source):
    """Indentation only: every 4 spaces at the start of a line, or right after
    a leading `//` (commented-out code), become one tab (tabsize=2 in the
    PDF); interior runs of spaces (alignment) are left alone."""
    return re.sub(r'^([\t ]*)(//)?([\t ]*)',
                  lambda m: m.group(1).replace('    ', '\t') + (m.group(2) or '') + m.group(3).replace('    ', '\t'),
                  source, flags=re.M)


COMMENT_TYPES = [
    ('/**', '*/'),
    ("'''", "'''"),
    ('"""', '"""'),
]

def find_start_comment(source, start=None):
    first = (-1, -1, None)
    for s, e in COMMENT_TYPES:
        i = source.find(s, start)
        if i != -1 and (i < first[0] or first[0] == -1):
            first = (i, i + len(s), e)
    return first

def parse_include(line):
    line = line.strip()
    if line.startswith("#include"):
        return line[8:].strip()
    return None

def addref(caption):
    """The header mark for the name line, and the name for the page-header state."""
    with open(HEADER_STATE, 'a', encoding='utf-8') as f:
        f.write(caption + "\n")
    return [r"\kactlref{%s}" % caption]

def emit_template(caption, block, includes, hashcaption, listingslang, source):
    """The LaTeX fragment for one template: header mark, the Description/Usage/
    Time/Memory fields, the include list and the tiny hash caption, then the
    listing."""
    out = addref(caption)
    for field, text in block:
        out.append(r"\def%s{%s}" % (field.lower(), text))
    if includes:
        out.append(r"\leftcaption{%s}" % pathescape(", ".join(includes)))
    if hashcaption:
        out.append(r"\rightcaption{%s}" % hashcaption)
    out.append(r"\begin{lstlisting}[caption={%s}, language=%s]" % (caption, listingslang))
    out.append(source)
    out.append(r"\end{lstlisting}")
    return out


def processwithcomments(caption, text, listingslang):
    includelist = []
    errors = []
    nlines = []
    for line in text.splitlines():
        if 'exclude-line' in line:
            if '<hash>' in line:
                errors.append("<hash> marker on an exclude-line (the line is not printed).")
            continue
        if 'include-line' in line:
            line = line.replace('// ', '', 1)
        had_comment = "///" in line
        keep_include = 'keep-include' in line
        # Remove /// comments
        stripped = line.split("///")[0].rstrip()
        if had_comment and '<hash>' in line and '<hash>' not in stripped:
            errors.append("<hash> marker inside a /// comment (it is not printed).")
        line = stripped
        # Remove '#pragma once' lines
        if line == "#pragma once":
            continue
        if had_comment and not line:
            continue
        # Check includes
        include = parse_include(line)
        if include is not None and not keep_include:
            includelist.append(include)
            continue
        nlines.append(line)
    # Remove and process multiline comments
    source = '\n'.join(nlines)
    nsource = ''
    start, start2, end_str = find_start_comment(source)
    end = 0
    commands = {}
    while start >= 0:
        nsource = nsource.rstrip() + source[end:start]
        end = source.find(end_str, start2)
        if end < start:
            errors.append("Invalid %s %s comments." % (source[start:start2], end_str))
            break
        comment = source[start2:end].strip()
        end += len(end_str)
        start, start2, end_str = find_start_comment(source, end)

        command = None
        value = ""
        for cline in comment.split('\n'):
            allow_command = False
            cline = cline.strip()
            if cline.startswith('*'):
                cline = cline[1:].strip()
                allow_command = True
            ind = cline.find(':')
            # "Word: ..." at the start of a "* " line is a command; "Word::" (C++
            # scope syntax in a continuation line, e.g. LCT::link) is not.
            if (allow_command and ind > 0 and ' ' not in cline[:ind] and cline[0].isalpha()
                    and cline[0].isupper() and not cline.startswith('::', ind)):
                if command:
                    if command not in KNOWN_COMMANDS:
                        errors.append("Unknown command: " + command + ".")
                    commands[command] = value.lstrip()
                command = cline[:ind]
                value = cline[ind + 1:].strip()
            else:
                value = value + '\n' + cline
        if command:
            if command not in KNOWN_COMMANDS:
                errors.append("Unknown command: " + command + ".")
            commands[command] = value.lstrip()
    for rcommand in sorted(set(REQUIRED_COMMANDS) - set(commands)):
        errors.append("Missing command: " + rcommand + ".")
    if end >= 0:
        nsource = nsource.rstrip() + source[end:]
    nsource = nsource.strip()

    hashed = listingslang in ['C++', 'Java']
    if not hashed and '<hash>' in nsource:
        errors.append("<hash> markers are only supported in C++/Java listings.")
    if errors:
        raise PreprocessorError(" ".join(errors))

    if hashed:
        nsource = hashmarkers(nsource)
        hsh = runhash(nsource) + ', ' if nsource else ''
    else:
        hsh = ''

    block = []
    if commands.get("Description"):
        block.append(("Description", escape(commands["Description"])))
    if commands.get("Usage"):
        block.append(("Usage", codeescape(commands["Usage"])))
    if commands.get("Time"):
        block.append(("Time", ordoescape(commands["Time"])))
    if commands.get("Memory"):
        block.append(("Memory", ordoescape(commands["Memory"])))
    hashcaption = "%s%d lines" % (hsh, len(nsource.split("\n"))) if nsource else ""
    return emit_template(caption, block, includelist, hashcaption, listingslang, tabify(nsource))

def processraw(caption, text, listingslang):
    source = text.strip()
    if listingslang == 'C++':
        # rawcpp (template.cpp): same prefix-hash markers and indentation as
        # the headers, but no doc comment and no hash caption.
        source = tabify(hashmarkers(source))
    elif '<hash>' in source:
        raise PreprocessorError("<hash> markers are only supported in C++/Java listings.")
    hashcaption = "%d lines" % len(source.split("\n")) if source else ""
    return emit_template(caption, [], [], hashcaption, listingslang, source)

# -l value (or file extension): processor and listings language
LANGUAGES = {
    'cpp': (processwithcomments, 'C++'),
    'cc': (processwithcomments, 'C++'),
    'c': (processwithcomments, 'C++'),
    'h': (processwithcomments, 'C++'),
    'hpp': (processwithcomments, 'C++'),
    'java': (processwithcomments, 'Java'),
    'py': (processwithcomments, 'Python'),
    'raw': (processraw, 'raw'),
    'rawcpp': (processraw, 'C++'),
    'sh': (processraw, 'bash'),
}


def print_header(data):
    """data: the last \\hdrmark placed on the page (see kactlpkg.sty), i.e. the
    last template that starts on it. Returns the LaTeX for the centre header:
    every not-yet-printed name up to that one, and drops them from the state."""
    until = data.strip()
    if not until or not os.path.exists(HEADER_STATE):
        # Nothing on this page, skip it.
        return ""
    with open(HEADER_STATE, encoding='utf-8') as f:
        lines = [x.strip() for x in f.readlines()]
    if until not in lines:
        # Nothing new on the page.
        return ""

    ind = lines.index(until) + 1
    header_length = len("".join(lines[:ind]))
    def adjust(name):
        return name if name.startswith('.') else name.split('.')[0]
    names = list(map(adjust, lines[:ind]))
    if header_length > 110:
        # Long lists wrap onto two centred \footnotesize lines (the header
        # box is 25pt high and bottom-aligned, so the second line grows
        # upward, towards the paper edge) instead of being shrunk to an
        # unreadable size; the box is narrower than \headwidth so it never
        # reaches the side headers. (\enspace is a kern and never breaks,
        # hence the \hspace.)
        output = (r"\parbox[b]{\dimexpr\headwidth-10cm\relax}{\centering"
                  r"\footnotesize\textbf{" + r"\hspace{.5em}".join(names) + "}}")
    else:
        output = r"\hspace{3mm}\textbf{" + r"\enspace{}".join(names) + "}"
    with open(HEADER_STATE, 'w', encoding='utf-8') as f:
        for line in lines[ind:]:
            f.write(line + "\n")
    return output + "\n"


USAGE = """usage (from the repo root):
  preprocessor.py -i <file> [-o <out.tex>] [-l <language>]
  preprocessor.py --print-header <mark> [-o <out.tex>]
languages: %s (default: the file extension)""" % ", ".join(sorted(LANGUAGES))

def write_output(outpath, text):
    if outpath:
        with open(outpath, 'w', encoding='utf-8') as f:
            f.write(text)
    else:
        sys.stdout.write(text)

def main():
    language = None
    caption = None
    inpath = None
    outpath = None
    header = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ho:i:l:", ["help", "output=", "input=", "language=", "print-header="])
    except getopt.GetoptError as err:
        print(err, file=sys.stderr)
        print(USAGE, file=sys.stderr)
        return 2
    for option, value in opts:
        if option in ("-h", "--help"):
            print(USAGE)
            return 0
        if option in ("-o", "--output"):
            outpath = value
        if option in ("-i", "--input"):
            inpath = value
            if language is None:
                language = value.rsplit('.', 1)[-1]
            if caption is None:
                caption = value.rsplit('/', 1)[-1]
        if option in ("-l", "--language"):
            language = value
        if option == "--print-header":
            header = value

    if header is not None:
        # The page header is \input on every page, so the file must always exist.
        write_output(outpath, print_header(header))
        return 0
    if inpath is None:
        print(USAGE, file=sys.stderr)
        return 2

    print(" * \x1b[1m{}\x1b[0m".format(caption))
    caption = pathescape(caption).strip()
    try:
        if language not in LANGUAGES:
            raise PreprocessorError("unknown language '%s' (use -l %s)" % (language, "|".join(sorted(LANGUAGES))))
        with open(inpath, encoding='utf-8') as f:
            text = f.read()
        process, listingslang = LANGUAGES[language]
        out = process(caption, text, listingslang)
    except PreprocessorError as err:
        print("%s: %s" % (caption, err), file=sys.stderr)
        out = [r"\kactlerror{%s: %s}" % (caption, texsafe(str(err)))]
    except Exception as err:
        traceback.print_exc()
        out = [r"\kactlerror{%s: %s: %s (see the terminal)}" % (caption, type(err).__name__, texsafe(str(err)))]
    write_output(outpath, "\n".join(out) + "\n")
    return 0

if __name__ == "__main__":
    sys.exit(main())
