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

import random, math
random.seed(1)                 # reproducible
random.random()                # float in [0, 1)
random.uniform(a, b)           # float in [a, b]
random.randint(a, b)           # int in [a, b], b included
random.randrange(a, b)         # int in [a, b)
random.choice(xs)              # one element
random.sample(xs, k)           # k without replacement
random.choices(xs, k=k)        # k with replacement
random.shuffle(xs)             # in place
random.getrandbits(64)

math.factorial(n)
math.comb(n, k)                # n choose k, exact (3.8+)
math.perm(n, k)                # n! / (n - k)!
math.gcd(a, b), math.lcm(a, b) # lcm is 3.9+
math.isqrt(n)                  # floor(sqrt(n)), exact
pow(a, b, m)                   # pow(a, -1, m): inverse (3.8+)
divmod(a, b)                   # (a // b, a % b)
math.log2(x), math.log10(x), math.log(x, base)
math.floor(x), math.ceil(x), round(x, 2)
math.hypot(x, y), math.atan2(y, x)
math.inf, math.pi, math.e
