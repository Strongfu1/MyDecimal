#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

typedef enum {
  s21_NORMAL_VALUE = 0,
  s21_INFINITY = 1,
  s21_NEGATIVE_INFINITY = 2,
  s21_NAN = 3,
} value_type_t;

typedef struct {
  unsigned int bits[4];
  value_type_t value_type;
} s21_decimal;

// MAIN FUNCTIONS
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
s21_decimal s21_add(s21_decimal, s21_decimal);
s21_decimal s21_sub(s21_decimal, s21_decimal);
s21_decimal s21_mul(s21_decimal, s21_decimal);
s21_decimal s21_div(s21_decimal, s21_decimal);
s21_decimal s21_mod(s21_decimal, s21_decimal);
int s21_from_decimal_to_int(s21_decimal, int*);
int s21_from_float_to_decimal(float, s21_decimal*);
int s21_from_int_to_decimal(int, s21_decimal*);
int s21_from_decimal_to_float(s21_decimal, float*);
s21_decimal s21_truncate(s21_decimal);
s21_decimal s21_floor(s21_decimal);
s21_decimal s21_round(s21_decimal);
s21_decimal s21_negate(s21_decimal);

// Function for tests
unsigned CheckSpecialValue(s21_decimal*, float);

#endif
