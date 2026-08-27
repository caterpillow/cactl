/**
 * Author: Simon Lindholm
 * Date: 2015-03-19
 * License: CC0
 * Source: me
 * Description: Paste inside a function with \texttt{from}, \texttt{to} in scope; body in F
 * must be straight-line (no break/continue, do not touch i). \texttt{\#undef F} after.
 */
#pragma once

#define F {...; ++i;}
int i = from;
while (i & 3 && i < to) F // for alignment, if needed
while (i + 4 <= to) { F F F F }
while (i < to) F
