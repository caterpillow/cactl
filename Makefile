# Run from the repo root: TEXINPUTS, the preprocessor and hash.sh use relative paths.
LATEXCMD = pdflatex -shell-escape -interaction=nonstopmode -halt-on-error -output-directory build/
export TEXINPUTS=.:content/tex/:
export max_print_line = 1048576

help:
	@echo "This makefile builds the notebook (kactl.pdf, a fork of KACTL)"
	@echo ""
	@echo "Available commands are:"
	@echo "	make fast		- to build kactl.pdf quickly (one pdflatex pass)"
	@echo "	make kactl		- to build kactl.pdf (two passes) and test-session.pdf"
	@echo "	make clean		- to remove the build/ products and temp files"
	@echo "	make veryclean		- to clean up and remove kactl.pdf/test-session.pdf"
	@echo "	make test		- to run all the stress tests in stress-tests/"
	@echo "	make test-compiles	- to test compiling all headers"
	@echo "	make showexcluded	- to show source files that are not in the pdf"
	@echo "	make widelines		- to list code lines wider than 63 chars (>70 overflows the column)"
	@echo "	make help		- to show this information"
	@echo ""
	@echo "For more information see the file 'doc/README'"

fast: | build
	$(LATEXCMD) content/kactl.tex
	cp build/kactl.pdf kactl.pdf

kactl: test-session.pdf | build
	$(LATEXCMD) content/kactl.tex && $(LATEXCMD) content/kactl.tex
	cp build/kactl.pdf kactl.pdf

clean:
	rm -f build/kactl.* build/test-session.* build/*.tmp header.tmp

veryclean: clean
	rm -f kactl.pdf test-session.pdf

.PHONY: help fast kactl clean veryclean test test-compiles showexcluded widelines

build:
	mkdir -p build/

test:
	./doc/scripts/run-all.sh .

test-compiles:
	./doc/scripts/compile-all.sh .

test-session.pdf: content/test-session/test-session.tex content/test-session/chapter.tex | build
	$(LATEXCMD) content/test-session/test-session.tex
	cp build/test-session.pdf test-session.pdf

# Source files under content/ (except the build machinery in content/tex) that
# no chapter.tex imports; \kactlimport[opts]{file} counts as imported.
showexcluded: | build
	grep -RoPh '^\s*\\kactlimport(\[[^]]*\])?\{\K[^}]*' content/ | sed 's|^|/|' > build/headers_included
	find ./content -path ./content/tex -prune -o \( -name "*.h" -o -name "*.py" -o -name "*.java" \) -print | grep -vFf build/headers_included

# Code lines wider than the 63-char guideline, measured as the pdf renders
# them (tabs and leading 4-space groups are 2 columns, /// comments and the
# doc comment are not printed); a column holds 70 chars.
widelines:
	@awk 'FNR == 1 { inblock = 0 } \
	  /^[\t ]*\/\*\*/ { inblock = 1 } \
	  inblock { if ($$0 ~ /\*\//) inblock = 0; next } \
	  { line = $$0; sub(/\/\/\/.*/, "", line); \
	    if ($$0 ~ /exclude-line/ || line == "#pragma once") next; \
	    match(line, /^[\t ]*(\/\/[\t ]*)?/); ind = substr(line, 1, RLENGTH); \
	    gsub(/    /, "\t", ind); line = ind substr(line, RLENGTH + 1); \
	    w = 0; n = length(line); \
	    for (i = 1; i <= n; i++) w += (substr(line, i, 1) == "\t") ? 2 - (w % 2) : 1; \
	    if (w > 63) printf "%s %s:%d: %d cols\n", (w > 70 ? "OVERFLOW" : "        "), FILENAME, FNR, w }' \
	  $$(find content -name '*.h' | sort)
