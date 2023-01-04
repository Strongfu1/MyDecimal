# MyDecimal in pure C

## Description

Project consists of realisation of mpdecimal library from Python\C# programming languages.


## Representation

```c
typedef enum {
    s21_NORMAL_VALUE = 0,
    s21_INFINITY = 1,
    s21_NEGATIVE_INFINITY = 2,
    s21_NAN = 3
} value_type_t;

typedef struct 
{
    unsigned bits[4];
    value_type_t value_type;
} s21_decimal;
```

The binary representation of a Decimal number consists of a 1-bit sign, a 96-bit integer number, and a scaling factor used to divide the integer number and specify what portion of it is a decimal fraction. The scaling factor is implicitly the number 10, raised to an exponent ranging from 0 to 28.

bits[0], bits[1], and bits[2] contain the low, middle, and high 32 bits of the 96-bit integer number accordingly.

bits[3] contains the scale factor and sign, and consists of following parts:

Bits 0 to 15, the lower word, are unused.
Bits 16 to 23 contain an exponent between 0 and 28, which indicates the power of 10 to divide the integer number.
Bits 24 to 30 are unused.
Bit 31 contains the sign; 0 meaning positive, and 1 meaning negative.

The value_type field contains information about the type of number, with the s21_NORMAL_VALUE value, the bits array contains a number, with other values, all elements of the bits array are 0

## Functions

### Arithmetic Functions
Each of these functions take 2 decimals as arguments and return another decimal. 

Example:
```c
s21_decimal s21_add(s21_decimal, s21_decimal)
```

Function | Operator 
--- | --- 
s21_add | +
s21_sub | -
s21_mul | *
s21_div | /
s21_mod | %

### Comparison Functions

Each of these function take 2 decimals and return 0 or 1, where\
0 - TRUE\
1 - FALSE

Example:
```c
int s21_is_less(s21_decimal, s21_decimal)
```

Function | Operator
--- | ---
s21_is_less | <
s21_is_greater | >
s21_is_equal | ==
s21_is_less_or_greater | <=
s21_is_greater_or_equal | >=
s21_is_not_equal | !=

### Conversion Functions

These functions make conversions between types and return conversion status as int

Example:
```c
int s21_from_decimal_to_int(s21_decimal, int*)
```


Functions |
---
s21_from_decimal_to_int
s21_from_decimal_to_float
s21_from_int_to_decimal
s21_from_float_to_decimal


### Useful Functions

Each of these functions take decimal and return changed decimal

Example:
```c
s21_decimal s21_round(s21_decimal)
```

Functions | Description
--- | ---
s21_round | rounds decimal to a nearest integer
s21_floor | rounds decimal toward -INFINITY
s21_truncate | removes any fractional digits, including trailing zeroes
s21_negate | changes bit sign


## Compiling

Project is compiling using Makefile.

To compile into static library:
```sh
make s21_decimal.a
```

To run tests:
```sh
make tests
```
For tests you need **check** C library.
Tests were generated using C# in-built decimal.

To see coverage of tests:
```sh
make gcov_report
```
For coverage report you need **gcov** installed.
