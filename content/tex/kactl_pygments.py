#!/usr/bin/env python3
"""Pygments lexer + colour style for the cactl contest C++ dialect.

Two things live here:

  KactlCppLexer   - CppLexer plus the template.cpp dialect (FOR/F0R/ROF, ll/vt/vi
                    aliases, pb/all/size macros, inf/INF/mod constants), a
                    call/definition heuristic for function names, and struct /
                    template-parameter names propagated to their use sites.
  VSCodeLightStyle - VS Code "Light+" palette mapped onto Pygments token types.
                    THE PALETTE LIVES IN `PALETTE` BELOW.

The module is also a drop-in `pygmentize` replacement so minted can use both
without installing anything:

    python3 content/tex/kactl_pygments.py -l kactlcpp -f latex ... file
    python3 content/tex/kactl_pygments.py -S kactlvscode -f latex

(Any other pygmentize arguments pass through unchanged.)  Plain pygmentize
also works with explicit class paths:

    pygmentize -x -l content/tex/kactl_pygments.py:KactlCppLexer -f latex file

NOTE: minted caches the generated LaTeX per listing keyed on (source, command
line); editing the *classification* rules below requires deleting
build/_minted-kactl/ so listings are re-lexed.  Editing PALETTE only requires
deleting build/_minted-kactl/kactlvscode.pygstyle (or the whole cache dir).
If the LaTeX-side palette overrides in kactlhl.sty are in use, PALETTE is
irrelevant and nothing needs purging.
"""

import os
import sys

from pygments.lexers.c_cpp import CppLexer
from pygments.style import Style
from pygments.token import (Comment, Error, Generic, Keyword, Name, Number,
                            Operator, Punctuation, String, Text, Token,
                            Whitespace)

__all__ = ['KactlCppLexer', 'VSCodeLightStyle']

# ---------------------------------------------------------------------------
# Dialect vocabulary (template.cpp / stress-tests/utilities/template.h)
# ---------------------------------------------------------------------------

# `using ll = ...`, `#define vt vector` etc.  -> Keyword.Type
TYPE_ALIASES = {'ll', 'db', 'vt', 'vi', 'vl', 'pi', 'pl', 'pii', 'ld', 'ull'}

# Loop macros -> Keyword.ControlFlow (same colour as `for`).
# (The legacy shims rep/sz from stress-tests/utilities/template.h are left
# out on purpose: `sz` is a common variable name, e.g. HLD.h.)
LOOP_MACROS = {'FOR', 'F0R', 'ROF'}

# Function-like / expression macros -> Name.Builtin (VS Code macro colour).
UTIL_MACROS = {'pb', 'all', 'size'}

# `#define f first` / `#define s second`: only when used as `.f` / `->s`
# (they are also perfectly ordinary variable names elsewhere, e.g. `int s`).
MEMBER_MACROS = {'f', 's'}

# Constants -> Name.Constant (VS Code "variable.other.constant" #0070C1).
CONSTANTS = {'inf', 'INF', 'mod', 'MOD', 'LLONG_MAX', 'LLONG_MIN', 'INT_MAX',
             'INT_MIN', 'DBL_MAX', 'EPS', 'M_PI', 'NPOS'}

# Standard library types -> Name.Class (VS Code support.type / entity.name.type).
STL_TYPES = {
    'vector', 'array', 'deque', 'list', 'forward_list', 'string', 'string_view',
    'map', 'set', 'multimap', 'multiset', 'unordered_map', 'unordered_set',
    'unordered_multimap', 'unordered_multiset', 'pair', 'tuple', 'stack',
    'queue', 'priority_queue', 'bitset', 'complex', 'optional', 'variant',
    'function', 'less', 'greater', 'hash', 'iterator', 'const_iterator',
    'reverse_iterator', 'initializer_list', 'numeric_limits', 'istream',
    'ostream', 'ifstream', 'ofstream', 'stringstream', 'istringstream',
    'ostringstream', 'mt19937', 'mt19937_64', 'random_device', 'uniform_int_distribution',
    'uniform_real_distribution', 'chrono', 'atomic', 'basic_string',
    'point_iterator', 'tree', 'gp_hash_table', 'cc_hash_table', 'null_type',
    'rb_tree_tag', 'tree_order_statistics_node_update', 'splay_tree_tag',
}

# Keywords that VS Code paints purple (keyword.control) instead of blue.
CONTROL_FLOW = {'if', 'else', 'for', 'while', 'do', 'switch', 'case',
                'default', 'break', 'continue', 'return', 'goto', 'try',
                'catch', 'throw', 'co_return', 'co_await', 'co_yield'}

# Custom token type (Pygments token types are created on attribute access;
# the LaTeX formatter names it `kControlFlow`).
Keyword.ControlFlow  # noqa: B018  (touch so it exists)

_TYPE_TOKENS = (Keyword.Type, Name.Class)


class KactlCppLexer(CppLexer):
    """C++ with the cactl contest-template dialect."""

    name = 'KACTL C++'
    aliases = ['kactlcpp', 'kactl']
    filenames = []          # never auto-selected by extension
    mimetypes = []
    priority = 0

    def get_tokens_unprocessed(self, text, stack=('root',)):
        toks = list(CppLexer.get_tokens_unprocessed(self, text, stack))
        toks = self._merge_preproc(toks)
        toks = self._split_defines(toks)

        # Names declared with struct/class/typename/enum anywhere in the
        # listing are types at every use site (Pygments only marks the
        # declaration).
        declared = {v for _, t, v in toks if t is Name.Class}
        # ... plus local aliases:  using vb = ...;   typedef ... vb;
        sig_toks = [(t, v) for _, t, v in toks
                    if not (t in Whitespace or t in Comment or t in Text)]
        for j, (t, v) in enumerate(sig_toks):
            if t is Keyword and v == 'using' and j + 2 < len(sig_toks) \
                    and sig_toks[j + 1][0] is Name and sig_toks[j + 2][1] == '=':
                declared.add(sig_toks[j + 1][1])
            elif t is Keyword and v == 'typedef':
                k = j + 1
                while k < len(sig_toks) and sig_toks[k][1] != ';':
                    k += 1
                if k - 1 > j and sig_toks[k - 1][0] is Name:
                    declared.add(sig_toks[k - 1][1])

        n = len(toks)
        # next significant (non-space, non-comment) token value for lookahead
        next_sig = [None] * n
        nxt = None
        for i in range(n - 1, -1, -1):
            next_sig[i] = nxt
            t = toks[i][1]
            if not (t in Whitespace or t in Comment or t in Text):
                nxt = toks[i][2]

        out = []
        prev1 = prev2 = (None, None)   # (token, value) of previous significant tokens
        in_decl = False                # saw `Type name(args)` since the last ; { }
        in_ctor_init = False           # between `Ctor(...) :` and `{`: `n(x), t(0)` are members
        for i, (idx, tok, val) in enumerate(toks):
            sig = not (tok in Whitespace or tok in Comment or tok in Text)
            if sig and val == ':' and prev1[1] == ')' and next_sig[i] != ':':
                in_ctor_init = True
            if tok is Keyword and val in CONTROL_FLOW:
                tok = Keyword.ControlFlow
            elif tok is Name:
                if val in TYPE_ALIASES:
                    tok = Keyword.Type
                elif val in LOOP_MACROS:
                    tok = Keyword.ControlFlow
                elif val in UTIL_MACROS:
                    tok = Name.Builtin
                elif val in MEMBER_MACROS and (
                        prev1[1] == '.' or (prev1[1] == '>' and prev2[1] == '-')):
                    tok = Name.Builtin
                elif val in CONSTANTS:
                    tok = Name.Constant
                elif val in declared or val in STL_TYPES:
                    tok = Name.Class
                elif next_sig[i] == '(':
                    # `foo(` is a call or a definition, unless it is a
                    # constructor-style variable initialisation:
                    #   vi rev(n);  static vt<ll> rt(2, 1);  vt<T> L(a), R(b);
                    if in_ctor_init and prev1[1] in (':', ','):
                        decl = True
                    else:
                        decl = (prev1[0] in _TYPE_TOKENS
                                or (prev1[1] == '>' and self._type_before_angle(toks, i))
                                or (in_decl and prev1[1] == ','))
                        # ... but `Type name(params) {` is a definition.
                        if decl and self._is_definition(toks, i):
                            decl = False
                    if decl:
                        in_decl = True
                    else:
                        tok = Name.Function
            if sig:
                if val in (';', '{', '}'):
                    in_decl = in_ctor_init = False
                prev2, prev1 = prev1, (tok, val)
            out.append((idx, tok, val))
        return out

    @staticmethod
    def _is_definition(toks, i):
        """toks[i] is a Name followed by `(`; return True if the token after
        the matching `)` starts a function body/qualifier (`{`, `const`,
        `override`, `noexcept`, `->`, ctor-init `:`)."""
        depth = 0
        j = i + 1
        n = len(toks)
        while j < n:
            v = toks[j][2]
            if v == '(':
                depth += 1
            elif v == ')':
                depth -= 1
                if depth == 0:
                    k = j + 1
                    while k < n and (toks[k][1] in Whitespace or toks[k][1] in Comment):
                        k += 1
                    return k < n and toks[k][2] in ('{', 'const', 'override', 'noexcept', '-', ':')
            elif v in (';', '{', '}'):
                return False
            j += 1
        return False

    @staticmethod
    def _type_before_angle(toks, i):
        """toks[i] is a Name whose previous significant token is `>`; return
        True if the matching `<` is preceded by a type token (`vt<ll> rt(`)."""
        depth = 0
        j = i - 1
        while j >= 0:
            t, v = toks[j][1], toks[j][2]
            if v == '>':
                depth += 1
            elif v == '<':
                depth -= 1
                if depth == 0:
                    k = j - 1
                    while k >= 0 and (toks[k][1] in Whitespace or toks[k][1] in Comment):
                        k -= 1
                    if k < 0:
                        return False
                    tk, vk = toks[k][1], toks[k][2]
                    return tk in _TYPE_TOKENS or vk in TYPE_ALIASES or vk in STL_TYPES
            elif v in (';', '{', '}', '(', ')'):
                return False
            j -= 1
        return False

    @staticmethod
    def _merge_preproc(toks):
        """The C lexer splits a directive line into several Comment.Preproc
        chunks (at every `/` and at `\\`-newline continuations); glue them
        back together so a multi-line #define is one token."""
        out = []
        for idx, tok, val in toks:
            if out and tok is Comment.Preproc and out[-1][1] is Comment.Preproc \
                    and (not out[-1][2].endswith('\n') or out[-1][2].endswith('\\\n')):
                pidx, _, pval = out[-1]
                out[-1] = (pidx, tok, pval + val)
            else:
                out.append((idx, tok, val))
        return out

    def _split_defines(self, toks):
        """`#define NAME(args) body` -> Comment.Preproc `#define`,
        Name.Builtin `NAME`, then the body lexed as code (VS Code does this)."""
        out = []
        for idx, tok, val in toks:
            if tok is Comment.Preproc and val.lstrip('# \t').startswith('define'):
                head, _, rest = val.partition('define')
                out.append((idx, Comment.Preproc, head + 'define'))
                pos = idx + len(head) + 6
                ws = rest[:len(rest) - len(rest.lstrip(' \t'))]
                if ws:
                    out.append((pos, Whitespace, ws))
                    pos += len(ws)
                rest = rest[len(ws):]
                name_len = 0
                while name_len < len(rest) and (rest[name_len].isalnum() or rest[name_len] == '_'):
                    name_len += 1
                if name_len:
                    out.append((pos, Name.Builtin, rest[:name_len]))
                    pos += name_len
                    rest = rest[name_len:]
                if rest.strip():
                    body = rest.rstrip('\n')
                    for j, t, v in CppLexer.get_tokens_unprocessed(self, body, ('statement',)):
                        out.append((pos + j, t, v))
                    pos += len(body)
                    rest = rest[len(body):]
                if rest:
                    out.append((pos, Comment.Preproc, rest))
            else:
                out.append((idx, tok, val))
        return out


# ---------------------------------------------------------------------------
# Palette: VS Code Light+ (edit here; see module docstring about the cache)
# ---------------------------------------------------------------------------
PALETTE = {
    'text':        '#000000',   # operators, punctuation, anything unclassified
    'keyword':     '#0000FF',   # const static struct template using ...
    'control':     '#AF00DB',   # if else for while return ...  + FOR/F0R/ROF
    'type':        '#267F99',   # int ll vi + struct/STL names.  (VS Code paints
                                # the primitives int/bool/void #0000FF; to copy
                                # that, remap Keyword.Type for the primitive set
                                # to Keyword in get_tokens_unprocessed.)
    'function':    '#795E26',   # calls and definitions
    'variable':    '#001080',   # every remaining identifier (VS Code: variable)
    'builtin':     '#0000FF',   # macros pb/all/size, true/false/nullptr
    'constant':    '#0070C1',   # inf INF mod LLONG_MAX   (VS Code readonly var)
    'number':      '#098658',
    'string':      '#A31515',
    'comment':     '#008000',
    'preproc':     '#AF00DB',   # #include #define #pragma
    'preprocfile': '#A31515',   # <bits/stdc++.h>
}
COMMENT_ITALIC = True   # current kactl commentstyle is italic; VS Code is not


class VSCodeLightStyle(Style):
    name = 'kactlvscode'
    background_color = '#ffffff'
    styles = {
        Token:                 PALETTE['text'],
        Text:                  PALETTE['text'],
        Whitespace:            PALETTE['text'],
        Error:                 PALETTE['text'],
        Operator:              PALETTE['text'],
        Punctuation:           PALETTE['text'],
        # italic goes on the leaves: Pygments' LaTeX output applies the whole
        # chain (\PYG{c+cp}{..}) so an italic `Comment` would leak into #include.
        Comment:               PALETTE['comment'],
        Comment.Single:        ('italic ' if COMMENT_ITALIC else '') + PALETTE['comment'],
        Comment.Multiline:     ('italic ' if COMMENT_ITALIC else '') + PALETTE['comment'],
        Comment.Preproc:       PALETTE['preproc'],
        Comment.PreprocFile:   PALETTE['preprocfile'],
        Keyword:               PALETTE['keyword'],
        Keyword.Declaration:   PALETTE['keyword'],
        Keyword.Reserved:      PALETTE['keyword'],
        Keyword.Constant:      PALETTE['keyword'],
        Keyword.ControlFlow:   PALETTE['control'],
        Keyword.Type:          PALETTE['type'],
        Name:                  PALETTE['variable'],
        Name.Variable:         PALETTE['variable'],
        Name.Attribute:        PALETTE['variable'],
        Name.Label:            PALETTE['text'],
        Name.Namespace:        PALETTE['type'],
        Name.Function:         PALETTE['function'],
        Name.Class:            PALETTE['type'],
        Name.Builtin:          PALETTE['builtin'],
        Name.Constant:         PALETTE['constant'],
        Number:                PALETTE['number'],
        String:                PALETTE['string'],
        String.Char:           PALETTE['string'],
        String.Escape:         PALETTE['string'],
        Generic:               PALETTE['text'],
    }


# ---------------------------------------------------------------------------
# `pygmentize` shim: registers the lexer as `kactlcpp` and the style as
# `kactlvscode`, then defers to pygments' own command line.
# ---------------------------------------------------------------------------
def main(argv=None):
    import pygments.lexers
    import pygments.styles
    sys.modules.setdefault('kactl_pygments', sys.modules[__name__])
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    pygments.lexers._lexer_cache[KactlCppLexer.name] = KactlCppLexer
    pygments.lexers.LEXERS['KactlCppLexer'] = (
        'kactl_pygments', KactlCppLexer.name, tuple(KactlCppLexer.aliases), (), ())
    pygments.styles._STYLE_NAME_TO_MODULE_MAP['kactlvscode'] = (
        'kactl_pygments', 'VSCodeLightStyle')
    from pygments.cmdline import main as pygmentize_main
    return pygmentize_main(['pygmentize'] + list(sys.argv[1:] if argv is None else argv))


if __name__ == '__main__':
    sys.exit(main())
