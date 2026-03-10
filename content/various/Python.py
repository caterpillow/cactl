/**
 * Author: caterpillow
 * Date: 2025-10-23
 * Description: In case you need to python crutch 
 * Status: true
 */

# might not need encoding='utf-8'
# write text
with open('in', 'w', encoding='utf-8') as f:
    f.write("Hello\n")
    f.write("Line 2\n")

# append text
with open('notes.txt', 'a', encoding='utf-8') as f:
    f.write("Appended line\n")

# read text
with open('notes.txt', 'r', encoding='utf-8') as f:
    all_text = f.read()           # whole file as str
    # or iterate lines:
    f.seek(0)
    for line in f:
        print(line.strip())

from decimal import Decimal, getcontext, localcontext, ROUND_HALF_UP, ROUND_HALF_EVEN
from fractions import Fraction

Decimal('0.1')
Decimal('123.456')
Decimal(10)

f = Fraction(1, 3)
Decimal(f.numerator) / Decimal(f.denominator) 
d = Decimal('1.23')
Decimal(d)               # returns a copy

ctx = getcontext()
print(ctx.prec)         # integer precision (significant digits)
print(ctx.rounding)     # default rounding mode

# set global precision (affects operations that follow)
getcontext().prec = 50
Decimal('2').sqrt()