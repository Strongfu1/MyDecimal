#include "s21_decimal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

unsigned getscale(s21_decimal* decimal);
s21_decimal mul10(s21_decimal decimal);
s21_decimal div10(s21_decimal* decimal);
unsigned int getbit(s21_decimal* decimal, int bitn);
void setbit(s21_decimal* decimal, int bitn, unsigned int value);
s21_decimal inversion(s21_decimal decimal);
s21_decimal minisub(s21_decimal decimal, s21_decimal dst);
s21_decimal minisum(s21_decimal s1, s21_decimal s2);
void BitSetValue(s21_decimal* decimal, unsigned value);
void lshift(s21_decimal* decimal, int shift);
void rshift(s21_decimal* decimal, int shift);

unsigned binarytouns(s21_decimal* decimal, unsigned bitn, unsigned num);
void norm(s21_decimal* decimal, s21_decimal* dec2);
unsigned is_zero(s21_decimal* decimal);
int GetFirstOne(s21_decimal* sd1);
unsigned FirstDigit(s21_decimal* s1);

typedef struct {
  unsigned int bits[7];
  value_type_t value_type;
} s21_superdecimal;

int sdGetFirstOne(s21_superdecimal* sd1);
void decimal_to_sdecimal(s21_decimal* decimal, s21_superdecimal* sdecimal);
void sdecimal_to_decimal(s21_superdecimal* sdecimal, s21_decimal* decimal);
unsigned int sdgetbit(s21_superdecimal* decimal, int bitn);
void sdsetbit(s21_superdecimal* decimal, int bitn, unsigned int value);
unsigned sdgetscale(s21_superdecimal* decimal);
void sdBitSetValue(s21_superdecimal* decimal, unsigned value);
void sdlshift(s21_superdecimal* decimal, int shift);
void sdrshift(s21_superdecimal* decimal, int shift);
unsigned sdbinarytouns(s21_superdecimal* decimal, unsigned bitn, unsigned num);
s21_superdecimal sdmul10(s21_superdecimal decimal);
s21_superdecimal sdminisum(s21_superdecimal s1, s21_superdecimal s2);
s21_superdecimal sddiv10(s21_superdecimal* decimal);
void sdnorm(s21_superdecimal* decimal, s21_superdecimal* dec2);
s21_superdecimal sdinversion(s21_superdecimal decimal);
s21_superdecimal sdminisub(s21_superdecimal decimal, s21_superdecimal dst);
int sd_is_equal(s21_superdecimal sd1, s21_superdecimal sd2);
unsigned sdFirstDigit(s21_superdecimal* sd1);
int sdFindBiggest(s21_superdecimal* sd1, s21_superdecimal* sd2);
s21_superdecimal sdnormsum(s21_superdecimal s1, s21_superdecimal s2);
s21_superdecimal sdDopCode(s21_superdecimal dec1);
unsigned sdis_zero(s21_superdecimal* decimal);
void Special_Decimal(s21_decimal* s1, unsigned value);

s21_decimal s21_sub(s21_decimal s1, s21_decimal s2) {
  if (s1.value_type == s21_NORMAL_VALUE && s2.value_type == s21_NORMAL_VALUE) {
    s1.value_type = s21_NORMAL_VALUE;
    int sign_1 = getbit(&s1, 127);
    int sign_2 = getbit(&s2, 127);
    s21_superdecimal sd1 = {{0, 0, 0, 0, 0, 0, 0}, 0},
                     sd2 = {{0, 0, 0, 0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&s1, &sd1);
    decimal_to_sdecimal(&s2, &sd2);
    sdnorm(&sd1, &sd2);
    if (sign_1 == 1 && sign_2 == 1) {
      if (sdFindBiggest(&sd1, &sd2) == 1) {
        sd1 = sdminisub(sd2, sd1);
        sdsetbit(&sd1, 223, 0);
      } else {
        sd1 = sdminisub(sd1, sd2);
        sdsetbit(&sd1, 223, 1);
      }
    } else if (sign_1 == 0 && sign_2 == 0) {
      if (sdFindBiggest(&sd1, &sd2) == 1) {
        sd1 = sdminisub(sd2, sd1);
        sdsetbit(&sd1, 223, 1);
      } else {
        sd1 = sdminisub(sd1, sd2);
        sdsetbit(&sd1, 223, 0);
      }
    } else if (sign_1 == 1 && sign_2 == 0) {
      sd1 = sdminisum(sd2, sd1);
      sdsetbit(&sd1, 223, 1);
    } else if (sign_1 == 0 && sign_2 == 1) {
      sd1 = sdminisum(sd2, sd1);
      sdsetbit(&sd1, 223, 0);
    }
    sdecimal_to_decimal(&sd1, &s1);
  } else if (((s1.value_type == s21_NORMAL_VALUE ||
               s1.value_type == s21_INFINITY) &&
              s2.value_type == s21_NEGATIVE_INFINITY) ||
             (s1.value_type == s21_INFINITY &&
              s2.value_type == s21_NORMAL_VALUE)) {
    Special_Decimal(&s1, s21_INFINITY);
  } else if ((s1.value_type == s21_NEGATIVE_INFINITY && s2.value_type < 2) ||
             (s1.value_type == s21_NORMAL_VALUE &&
              s2.value_type == s21_INFINITY)) {
    Special_Decimal(&s1, s21_NEGATIVE_INFINITY);
  } else {
    Special_Decimal(&s1, s21_NAN);
  }
  return s1;
}

int s21_is_equal(s21_decimal s1, s21_decimal s2) {
  int ret = 0;
  if (s1.value_type == s21_NAN || s2.value_type == s21_NAN) {
    ret = 1;
  } else if (s1.value_type != s2.value_type) {
    ret = 1;
  } else if (s1.value_type == 0) {
    unsigned zero = 1;
    s21_superdecimal sd1 = {{0, 0, 0, 0}, 0}, sd2 = {{0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&s1, &sd1);
    decimal_to_sdecimal(&s2, &sd2);
    sdnorm(&sd1, &sd2);
    for (int i = 5; i >= 0 && ret != 1; i--) {
      if (sd1.bits[i] != sd2.bits[i]) ret = 1;
      if (sd1.bits[i] || sd2.bits[i]) zero = 0;
    }
    if (zero) {
      ret = 0;
    } else {
      if (getbit(&s1, 127) != getbit(&s2, 127)) ret = 1;
    }
  }
  return ret;
}

int s21_is_not_equal(s21_decimal s1, s21_decimal s2) {
  return !s21_is_equal(s1, s2);
}

int s21_is_less(s21_decimal s1, s21_decimal s2) {
  int ret = 1;
  if (s1.value_type == s21_NAN || s2.value_type == s21_NAN) {
    ret = 1;
  } else if (s1.value_type == s21_NEGATIVE_INFINITY &&
             s2.value_type != s21_NEGATIVE_INFINITY) {
    ret = 0;
  } else if (s2.value_type == s21_INFINITY && s1.value_type != s21_INFINITY) {
    ret = 0;
  } else if (s1.value_type == s21_INFINITY && s2.value_type != s21_INFINITY) {
    ret = 1;
  } else {
    s21_superdecimal sd1 = {{0, 0, 0, 0}, 0}, sd2 = {{0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&s1, &sd1);
    decimal_to_sdecimal(&s2, &sd2);
    sdnorm(&sd1, &sd2);
    unsigned sign1 = getbit(&s1, 127);
    unsigned sign2 = getbit(&s2, 127);
    if (sign1 == sign2) {
      if (!sign1) {
        for (int i = 5; i >= 0 && ret == 1; i--) {
          if (sd1.bits[i] < sd2.bits[i]) ret = 0;
          if (sd1.bits[i] > sd2.bits[i]) break;
        }
      } else {
        for (int i = 5; i >= 0 && ret == 1; i--) {
          if (sd1.bits[i] > sd2.bits[i]) ret = 0;
          if (sd1.bits[i] < sd2.bits[i]) break;
        }
      }
    } else {
      if (sign1 && (!is_zero(&s1) || !is_zero(&s2))) ret = 0;
    }
  }
  return ret;
}

int s21_is_greater(s21_decimal s1, s21_decimal s2) {
  int ret = 0;
  if (s1.value_type == s21_NAN || s2.value_type == s21_NAN) {
    ret = 1;
  } else if (s1.value_type == s21_INFINITY) {
    if (s2.value_type != s21_INFINITY) {
      ret = 0;
    } else {
      ret = 1;
    }
  } else {
    ret = !s21_is_less(s1, s2) || !s21_is_equal(s1, s2);
  }
  return ret;
}

int s21_is_less_or_equal(s21_decimal s1, s21_decimal s2) {
  return (s1.value_type == s21_NAN || s2.value_type == s21_NAN)
             ? 1
             : !s21_is_greater(s1, s2);
}

int s21_is_greater_or_equal(s21_decimal s1, s21_decimal s2) {
  return (s1.value_type == s21_NAN || s2.value_type == s21_NAN)
             ? 1
             : !s21_is_less(s1, s2);
}

s21_decimal s21_truncate(s21_decimal decimal) {
  if (decimal.value_type == s21_NORMAL_VALUE) {
    int scale = getscale(&decimal);
    while (scale != 0) {
      decimal = div10(&decimal);
      decimal.bits[3] -= 1 << 16;
      scale--;
    }
  }
  return decimal;
}

s21_decimal s21_floor(s21_decimal decimal) {
  if (decimal.value_type == s21_NORMAL_VALUE) {
    int scale = getscale(&decimal);
    if (scale) {
      int NotZero = 0;
      while (scale != 0) {
        if (FirstDigit(&decimal)) NotZero = 1;
        decimal = div10(&decimal);
        decimal.bits[3] -= 1 << 16;
        scale--;
      }
      if (NotZero && decimal.bits[3] >> 31) {
        s21_decimal one = {{1, 0, 0, 0}, 0};
        decimal = minisum(decimal, one);
      }
    }
  }
  return decimal;
}

s21_decimal s21_round(s21_decimal dec) {
  if (dec.value_type == s21_NORMAL_VALUE) {
    int MoreThanFive = 0, Fdigit = 0, Scale = getscale(&dec);
    while (Scale) {
      if (Fdigit) MoreThanFive = 1;
      Fdigit = FirstDigit(&dec);
      dec = div10(&dec);
      dec.bits[3] -= 1 << 16;
      Scale--;
    }
    if (Fdigit > 5 || (Fdigit == 5 && MoreThanFive) ||
        (FirstDigit(&dec) % 2 && Fdigit == 5)) {
      s21_decimal one = {{1, 0, 0, 0}, 0};
      dec = minisum(dec, one);
    }
  }
  return dec;
}

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  int result = 1;
  if (dst != NULL) {
    result = 0;
    BitSetValue(dst, 0);
    dst->bits[3] = 0;
    if (src < 0) {
      setbit(dst, 127, 1);
      src = -src;
    }
    dst->bits[0] = src;
  }
  return result;
}

void Special_Decimal(s21_decimal* s1, unsigned value) {
  s1->value_type = value;
  BitSetValue(s1, 0);
  s1->bits[3] = 0;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  int result = 0;
  float Max = 7.922816E+28, Min = 1e-28;
  if (dst == NULL || (fabs(src) > Max && !isinf(src)) ||
      (fabs(src) < Min && !isinf(src))) {
    if (fabs(src) > Max && isinf(src)) {
      BitSetValue(dst, 0);
      dst->value_type = 1 + getbit(dst, 127);
    }
    if (fabs(src) < Min && !isinf(src)) {
      BitSetValue(dst, 0);
      dst->value_type = 0;
    }
    result = 1;
  } else {
    dst->value_type = 0;
    unsigned bits = *(unsigned*)&src;
    int exp = (bits << 1 >> 24);
    if (exp == 255) {
      BitSetValue(dst, 0);
      dst->value_type = 1 + (bits >> 31) + (bits << 9 >> 30);
    } else {
      s21_superdecimal buf = {{0, 0, 0, 0, 0, 0, 0}, 0};
      int flag = 0;
      int okr = 0;
      if (fabs(src) < 1e-15) {
        flag = (int)fabs(log10(src)) - 6;
        src = src * pow(10, flag);
        bits = *(unsigned*)&src;
        exp = (bits << 1 >> 24);
      }
      s21_superdecimal mantiss = {{5, 0, 0, 0, 0, 0, 1 << 16}, 0};
      sdsetbit(&buf, 223, bits >> 31);
      exp -= 127;
      bits = bits << 9;
      if (exp >= 0) {
        sdsetbit(&buf, exp, 1);
        for (int i = 0; i < exp && i < 23; i++) {
          sdsetbit(&buf, exp - i - 1, bits << i >> 31);
        }
      } else {
        okr = 1;
        while (exp < -1) {
          mantiss = sdmul10(mantiss);
          mantiss.bits[6] += 1 << 16;
          sdrshift(&mantiss, 1);
          exp++;
        }
        sdnorm(&buf, &mantiss);
        buf = sdminisum(buf, mantiss);
        mantiss = sdmul10(mantiss);
        mantiss.bits[6] += 1 << 16;
        sdrshift(&mantiss, 1);
        exp++;
      }
      for (unsigned i = exp; i < 23; i++) {
        unsigned bit = bits << i >> 31;
        if (bit) {
          sdnorm(&buf, &mantiss);
          buf = sdminisum(buf, mantiss);
        }
        mantiss = sdmul10(mantiss);
        mantiss.bits[6] += 1 << 16;
        sdrshift(&mantiss, 1);
      }
      if (flag) buf.bits[6] += flag << 16;
      sdecimal_to_decimal(&buf, dst);
      if (flag || okr) {
        unsigned MoreThanFive = 0, Five = 0;
        while (getscale(dst) &&
               ((dst->bits[2] || dst->bits[1] || log10(dst->bits[0]) > 7) ||
                (!FirstDigit(dst) && dst->bits[0]))) {
          if (Five) MoreThanFive = 1;
          Five = FirstDigit(dst);
          *dst = div10(dst);
          dst->bits[3] -= 1 << 16;
          if (!dst->bits[2] && !dst->bits[1] &&
              (((int)log10(dst->bits[0])) == 6) &&
              (Five > 5 || (MoreThanFive && Five == 5))) {
            s21_decimal one = {{1, 0, 0, 0}, 0};
            *dst = minisum(*dst, one);
          }
        }
        if (!((dst->bits[0] + 1) % 10)) {
          s21_decimal one = {{1, 0, 0, 0}, 0};
          *dst = minisum(*dst, one);
          while (getscale(dst) && !FirstDigit(dst)) {
            *dst = div10(dst);
            dst->bits[3] -= 1 << 16;
          }
        }
      } else {
        unsigned sdvig = 0;
        unsigned okrug = 0;
        while (dst->bits[2] || dst->bits[1] || log10(dst->bits[0]) > 7) {
          sdvig++;
          if (FirstDigit(dst) >= 5)
            okrug = 1;
          else
            okrug = 0;
          *dst = div10(dst);
        }
        if (okrug) {
          s21_decimal one = {{1, 0, 0, 0}, 0};
          *dst = minisum(*dst, one);
        }
        for (unsigned i = 0; i < sdvig; i++) {
          *dst = mul10(*dst);
        }
      }
    }
    while (getscale(dst) && !FirstDigit(dst)) {
      *dst = div10(dst);
      dst->bits[3] -= 1 << 16;
    }
  }
  return result;
}

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  int result = 0;
  if (src.value_type == s21_NORMAL_VALUE && dst != NULL && !src.bits[1] &&
      !src.bits[2] && src.bits[0] <= 2147483647) {
    src = s21_truncate(src);
    *dst = 0;
    unsigned int res = 0;
    for (int i = 0; i < 32; i++) res += (getbit(&src, i) << i);
    *dst = res;
    if (getbit(&src, 127)) *dst *= -1;
  } else {
    result = 1;
  }
  return result;
}

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  int result = 1;
  if (dst != NULL) {
    unsigned bits = 0;
    if (src.value_type == s21_NORMAL_VALUE) {
      result = 0;
      s21_decimal trunc = s21_truncate(src);
      unsigned u = GetFirstOne(&trunc);
      bits = getbit(&src, 127) << 31;
      if (!is_zero(&src)) {
        unsigned roundup = 0;
        int i = 22;
        if (u || trunc.bits[0]) {
          bits |= (u + 127) << 23;
          for (; u > 0 && i >= 0; u--, i--) {
            bits |= getbit(&trunc, u - 1) << i;
          }
          if (i < 0 && getbit(&trunc, u - 1)) roundup = 1;
          norm(&trunc, &src);
          src = minisub(src, trunc);
        } else {
          unsigned exp = 0;
          s21_superdecimal sd = {{0, 0, 0, 0, 0, 0, 0}, 0};
          decimal_to_sdecimal(&src, &sd);
          while (!trunc.bits[0]) {
            sdlshift(&sd, 1);
            sdecimal_to_decimal(&sd, &trunc);
            trunc = s21_truncate(trunc);
            exp++;
          }
          sdecimal_to_decimal(&sd, &src);
          norm(&trunc, &src);
          src = minisub(src, trunc);
          bits |= (127 - exp) << 23;
        }
        while (i >= 0 && !is_zero(&src)) {
          lshift(&src, 1);
          trunc = s21_truncate(src);
          if (trunc.bits[0]) {
            bits |= 1 << i;
            norm(&src, &trunc);
            src = minisub(src, trunc);
          }
          i--;
        }
        if (!u && !is_zero(&src)) {
          lshift(&src, 1);
          trunc = s21_truncate(src);
          if (trunc.bits[0]) roundup = 1;
        }
        if (roundup) bits += 1;
      }
    } else {
      bits = 255 << 23;
      if (src.value_type == s21_NEGATIVE_INFINITY) bits |= ((unsigned)1 << 31);
      if (src.value_type == s21_NAN) bits |= 1 << 22;
    }
    *dst = *(float*)&bits;
  }
  return result;
}

s21_decimal s21_add(s21_decimal s1, s21_decimal s2) {
  if (s1.value_type == s21_NAN || s2.value_type == s21_NAN) {
    Special_Decimal(&s1, s21_NAN);
  } else if (s1.value_type == s21_INFINITY ||
             s1.value_type == s21_NEGATIVE_INFINITY ||
             s2.value_type == s21_NEGATIVE_INFINITY ||
             s2.value_type == s21_INFINITY) {
    if ((s1.value_type == s21_INFINITY &&
         s2.value_type == s21_NEGATIVE_INFINITY) ||
        (s1.value_type == s21_NEGATIVE_INFINITY &&
         s2.value_type == s21_INFINITY)) {
      Special_Decimal(&s1, s21_NAN);
    } else if (s1.value_type != s2.value_type) {
      Special_Decimal(&s1, s1.value_type + s2.value_type);
    }
  } else {
    int sign = (getbit(&s1, 127) == getbit(&s2, 127));
    s21_superdecimal sd1 = {{0, 0, 0, 0, 0, 0, 0}, 0},
                     sd2 = {{0, 0, 0, 0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&s1, &sd1);
    decimal_to_sdecimal(&s2, &sd2);
    sdnorm(&sd1, &sd2);
    if (sign) {
      sd1 = sdminisum(sd1, sd2);
    } else {
      if (sdFindBiggest(&sd1, &sd2)) {
        sign = sdgetbit(&sd2, 223);
        sd1 = sdminisub(sd2, sd1);
        sdsetbit(&sd1, 223, sign);
      } else {
        sign = sdgetbit(&sd1, 223);
        sd1 = sdminisub(sd1, sd2);
        sdsetbit(&sd1, 223, sign);
      }
    }
    sdecimal_to_decimal(&sd1, &s1);
  }
  return s1;
}

s21_decimal s21_mul(s21_decimal mul1, s21_decimal mul2) {
  if (mul1.value_type == s21_NAN || mul2.value_type == s21_NAN) {
    Special_Decimal(&mul1, s21_NAN);
  } else if (mul1.value_type == s21_INFINITY ||
             mul1.value_type == s21_NEGATIVE_INFINITY ||
             mul2.value_type == s21_NEGATIVE_INFINITY ||
             mul2.value_type == s21_INFINITY) {
    if (((mul1.value_type == s21_INFINITY ||
          mul1.value_type == s21_NEGATIVE_INFINITY) &&
         mul2.value_type == s21_NORMAL_VALUE && is_zero(&mul2)) ||
        ((mul2.value_type == s21_INFINITY ||
          mul2.value_type == s21_NEGATIVE_INFINITY) &&
         mul1.value_type == s21_NORMAL_VALUE && is_zero(&mul1))) {
      Special_Decimal(&mul1, s21_NAN);
    } else if (mul1.value_type == s21_NORMAL_VALUE) {
      if (getbit(&mul1, 127)) {
        Special_Decimal(&mul1, mul2.value_type == 1 ? 2 : 1);
      } else {
        Special_Decimal(&mul1, mul2.value_type);
      }
    } else if (mul2.value_type == s21_NORMAL_VALUE) {
      if (getbit(&mul2, 127)) {
        Special_Decimal(&mul1, mul1.value_type == 1 ? 2 : 1);
      } else {
        Special_Decimal(&mul1, mul1.value_type);
      }
    } else {
      Special_Decimal(&mul1, (mul1.value_type + mul2.value_type) % 2 + 1);
    }
  } else {
    s21_superdecimal sd1 = {{0, 0, 0, 0, 0, 0, 0}, 0},
                     answer = {{0, 0, 0, 0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&mul1, &sd1);
    for (unsigned i = 0; i < 96; i++) {
      if (getbit(&mul2, i)) {
        answer = sdminisum(answer, sd1);
      }
      sdlshift(&sd1, 1);
    }
    sdsetbit(&answer, 223, getbit(&mul1, 127) ^ getbit(&mul2, 127));
    unsigned scale = getscale(&mul1) + getscale(&mul2);
    answer.bits[6] += scale << 16;
    sdecimal_to_decimal(&answer, &mul1);
  }
  return mul1;
}

s21_decimal s21_div(s21_decimal s1, s21_decimal s2) {
  if (s1.value_type == s21_NAN || s2.value_type == s21_NAN) {
    Special_Decimal(&s1, s21_NAN);
  } else if (s1.value_type == s21_NORMAL_VALUE &&
             s2.value_type != s21_NORMAL_VALUE) {
    Special_Decimal(&s1, s21_NORMAL_VALUE);
  } else if (!is_zero(&s1) && is_zero(&s2)) {
    Special_Decimal(&s1, ((getbit(&s1, 127) || s1.value_type == 2) +
                          (getbit(&s2, 127) || s2.value_type == 2)) == 0
                             ? 1
                             : 2);
  } else if (s1.value_type && !s2.value_type) {
    Special_Decimal(&s1, s1.value_type);
  } else if (s1.value_type == s21_NORMAL_VALUE &&
             s2.value_type == s21_NORMAL_VALUE && !is_zero(&s1)) {
    s21_superdecimal sd1 = {{0, 0, 0, 0, 0, 0, 0}, 0}, sd2 = sd1;
    s21_superdecimal answer = {
        {0, 0, 0, 0, 0, 0, (getbit(&s1, 127) ^ getbit(&s2, 127)) << 31}, 0};
    s21_superdecimal FractionalPart = {{0, 0, 0, 0, 0, 0, 0}, 0};
    decimal_to_sdecimal(&s1, &sd1);
    decimal_to_sdecimal(&s2, &sd2);
    sdnorm(&sd1, &sd2);
    int power = 1;
    if (sdFindBiggest(&sd1, &sd2)) {
      FractionalPart.bits[6] += power++ << 16;
    }
    int shift = 0;
    for (int i = 0; i <= 35 && !sdis_zero(&sd1); i++) {
      if (sdFindBiggest(&sd1, &sd2)) {
        sd1 = sdmul10(sd1);
      } else {
        while (!sdFindBiggest(&sd1, &sd2)) {
          sd2 = sdmul10(sd2);
          shift++;
        }
        if (sdFindBiggest(&sd1, &sd2)) {
          sd2 = sddiv10(&sd2);
          shift--;
        }
      }
      unsigned ans = 0;
      while (!sdis_zero(&sd1) && !sdFindBiggest(&sd1, &sd2)) {
        sd1 = sdminisub(sd1, sd2);
        ans++;
      }
      FractionalPart.bits[0] = ans;
      for (int j = 0; j < shift; j++) {
        FractionalPart = sdmul10(FractionalPart);
      }
      sdnorm(&answer, &FractionalPart);
      answer = sdminisum(answer, FractionalPart);
      sdBitSetValue(&FractionalPart, 0);
      if (!shift)
        FractionalPart.bits[6] = power++ << 16;
      else
        shift--;
    }
    sdecimal_to_decimal(&answer, &s1);
  } else {
    Special_Decimal(&s1, s21_NAN);
  }
  return s1;
}

s21_decimal s21_mod(s21_decimal dec1, s21_decimal dec2) {
  if (dec1.value_type == s21_NORMAL_VALUE &&
      dec2.value_type == s21_NORMAL_VALUE && !is_zero(&dec2)) {
    s21_superdecimal sd1 = {{0, 0, 0, 0, 0, 0, 0}, 0}, sd2 = sd1;
    decimal_to_sdecimal(&dec1, &sd1);
    decimal_to_sdecimal(&dec2, &sd2);
    sdnorm(&sd1, &sd2);
    if (!sdFindBiggest(&sd1, &sd2)) {
      int shift = sdGetFirstOne(&sd1) - sdGetFirstOne(&sd2);
      s21_superdecimal shiftDec2 = sd2;
      sdlshift(&shiftDec2, shift);
      s21_superdecimal subDec2 = sdDopCode(shiftDec2);
      sd1 = sdnormsum(sd1, subDec2);
      int reg;
      reg = !sdgetbit(&sd1, 191);
      int k = 0;
      int flag = 0;
      for (int i = 0; i < shift && !sdFindBiggest(&sd1, &sd2); i++, k++) {
        flag = 1;
        sdlshift(&sd1, 1);
        if (reg) {
          sd1 = sdnormsum(sd1, subDec2);
        } else {
          sd1 = sdnormsum(sd1, shiftDec2);
        }
        reg = !sdgetbit(&sd1, 191);
      }
      if (flag && sdFindBiggest(&sd1, &sd2)) {
        sdlshift(&sd1, shift - k);
      }
      if (sdgetbit(&sd1, 191)) {
        sd1 = sdnormsum(sd1, shiftDec2);
      }
      if (flag) sdrshift(&sd1, shift);
      sdecimal_to_decimal(&sd1, &dec1);
    }
  } else if (dec1.value_type != s21_NORMAL_VALUE ||
             dec2.value_type == s21_NAN ||
             (is_zero(&dec2) && dec2.value_type == s21_NORMAL_VALUE)) {
    Special_Decimal(&dec1, s21_NAN);
  }
  return dec1;
}

s21_decimal s21_negate(s21_decimal decimal) {
  if (decimal.value_type == s21_INFINITY ||
      decimal.value_type == s21_NEGATIVE_INFINITY)
    decimal.value_type = decimal.value_type % 2 + 1;
  else if (decimal.value_type == s21_NORMAL_VALUE)
    setbit(&decimal, 127, !getbit(&decimal, 127));
  return decimal;
}

// SUPPORT FUNCTIONS

unsigned getscale(s21_decimal* decimal) {
  return (decimal->bits[3] << 8) >> 24;
}

s21_decimal minisum(s21_decimal s1, s21_decimal s2) {
  unsigned int buf = 0;
  s21_decimal res1 = s1;
  for (int l = 0; l < 3; l++) res1.bits[l] = 0;
  for (int i = 0; i < 96; i++) {
    unsigned int b1, b2, res;
    b1 = getbit(&s1, i);
    b2 = getbit(&s2, i);
    res = (b1 + b2 + buf) % 2;
    buf = (b1 + b2 + buf) / 2;
    setbit(&res1, i, res);
  }
  if (buf) {
    s21_decimal one = {{1, 0, 0, 0}, 0};
    res1 = minisum(res1, one);
  }
  return res1;
}

void lshift(s21_decimal* decimal, int shift) {
  int i = 95 - shift;
  while (i >= 0) {
    unsigned int bit = getbit(decimal, i);
    setbit(decimal, (i + shift), bit);
    i--;
  }
  while (shift > 0) {
    setbit(decimal, (shift - 1), 0);
    shift--;
  }
}

void rshift(s21_decimal* decimal, int shift) {
  int i = shift;
  while (i <= 95) {
    unsigned int bit = getbit(decimal, i);
    setbit(decimal, (i - shift), bit);
    i++;
  }
  while (shift > 0) {
    setbit(decimal, (96 - shift), 0);
    shift--;
  }
}

s21_decimal mul10(s21_decimal decimal) {
  s21_decimal decimal2 = decimal;
  lshift(&decimal2, 1);
  lshift(&decimal, 3);
  decimal = minisum(decimal, decimal2);
  return decimal;
}

unsigned int getbit(s21_decimal* decimal, int bitn) {
  return (decimal->bits[bitn / 32] >> bitn % 32) % 2;
}

void setbit(s21_decimal* decimal, int bitn, unsigned int value) {
  unsigned int dvalue = getbit(decimal, bitn);
  if (dvalue != value) {
    if (value == 0) {
      decimal->bits[bitn / 32] -= (1 << bitn % 32);
    } else {
      decimal->bits[bitn / 32] += (1 << bitn % 32);
    }
  }
}

s21_decimal minisub(s21_decimal decimal, s21_decimal dst) {
  norm(&decimal, &dst);
  s21_decimal invdecimal;
  if (s21_is_equal(decimal, dst)) {
    invdecimal = inversion(dst);
    invdecimal = minisum(invdecimal, decimal);
  } else {
    BitSetValue(&invdecimal, 0);
  }
  return invdecimal;
}

s21_decimal inversion(s21_decimal decimal) {
  decimal.bits[0] = ~decimal.bits[0];
  decimal.bits[1] = ~decimal.bits[1];
  decimal.bits[2] = ~decimal.bits[2];
  return decimal;
}

void BitSetValue(s21_decimal* decimal, unsigned value) {
  if (value == 1) value = -1;
  for (int l = 0; l < 3; l++) decimal->bits[l] = value;
  decimal->bits[3] = 0;
}

void norm(s21_decimal* decimal, s21_decimal* dec2) {
  unsigned s1 = getscale(decimal);
  unsigned s2 = getscale(dec2);
  if (s1 < s2) {
    while (s2 != s1) {
      *decimal = mul10(*decimal);
      decimal->bits[3] += 1 << 16;
      s1++;
    }
  } else if (s1 > s2) {
    while (s2 != s1) {
      *dec2 = mul10(*dec2);
      dec2->bits[3] += 1 << 16;
      s2++;
    }
  }
}

s21_decimal div10(s21_decimal* decimal) {
  s21_decimal result = {0};
  BitSetValue(&result, 0);
  result.bits[3] = decimal->bits[3];
  int i = 95;
  unsigned g = GetFirstOne(decimal);
  while (i != -1 && !getbit(decimal, i)) i--;
  unsigned uns = binarytouns(decimal, i, (i >= 3) ? 4 : i + 1);
  i -= (i >= 3) ? 4 : i + 1;
  if (uns >= 10) {
    uns -= 10;
    setbit(&result, g--, 1);
  } else {
    setbit(&result, g--, 0);
  }
  while (i >= 0) {
    if (uns < 10) {
      uns *= 2;
      uns += getbit(decimal, i--);
    }
    if (uns >= 10) {
      uns -= 10;
      setbit(&result, g--, 1);
    } else {
      setbit(&result, g--, 0);
    }
  }
  rshift(&result, g + 1);
  return result;
}

unsigned binarytouns(s21_decimal* decimal, unsigned bitn, unsigned num) {
  unsigned result = 0;
  if (num - 1 <= bitn) {
    unsigned mnoj = 1;
    for (; num > 0; num--) {
      result += getbit(decimal, bitn - num + 1) * mnoj;
      mnoj *= 2;
    }
  }
  return result;
}

int GetFirstOne(s21_decimal* sd1) {
  int l = 2;
  int bitn;
  for (; l > 0 && !sd1->bits[l]; l--) {
  }
  if (!sd1->bits[l]) {
    bitn = 0;
  } else {
    bitn = (l + 1) * 32 - 1;
    while (!getbit(sd1, bitn)) bitn--;
  }
  return bitn;
}

unsigned CheckSpecialValue(s21_decimal* s1, float f1) {
  if (f1 != INFINITY && f1 != -INFINITY && f1 == f1 && !s1->value_type)
    return 0;
  if (f1 == INFINITY && s1->value_type == 1) return 0;
  if (f1 == -INFINITY && s1->value_type == 2) return 0;
  if (f1 != f1 && s1->value_type == 3) return 0;
  return 1;
}

// SUPERDECIMAL FUNCTIONS

int sdGetFirstOne(s21_superdecimal* sd1) {
  int l = 5;
  int bitn;
  for (; l > 0 && !sd1->bits[l]; l--) {
  }
  if (!sd1->bits[l]) {
    bitn = 0;
  } else {
    bitn = (l + 1) * 32 - 1;
    while (!sdgetbit(sd1, bitn)) bitn--;
  }
  return bitn;
}

void decimal_to_sdecimal(s21_decimal* decimal, s21_superdecimal* sdecimal) {
  sdecimal->bits[0] = decimal->bits[0];
  sdecimal->bits[1] = decimal->bits[1];
  sdecimal->bits[2] = decimal->bits[2];
  sdecimal->bits[6] = decimal->bits[3];
}

void sdecimal_to_decimal(s21_superdecimal* sd1, s21_decimal* decimal) {
  int MoreThanFive = 0, Fdigit = 0, Scale = sdgetscale(sd1);
  while (Scale > 28 ||
         (Scale && (sd1->bits[3] || sd1->bits[4] || sd1->bits[5]))) {
    if (Fdigit) MoreThanFive = 1;
    Fdigit = sdFirstDigit(sd1);
    *sd1 = sddiv10(sd1);
    sd1->bits[6] -= 1 << 16;
    Scale--;
  }
  if (Fdigit > 5 || (Fdigit == 5 && MoreThanFive) ||
      (sdFirstDigit(sd1) % 2 && Fdigit == 5)) {
    s21_superdecimal one = {{1, 0, 0, 0, 0, 0, 0}, 0};
    *sd1 = sdminisum(*sd1, one);
  }
  while (!sdFirstDigit(sd1) && sdgetscale(sd1)) {
    *sd1 = sddiv10(sd1);
    sd1->bits[6] -= 1 << 16;
  }
  if (sd1->bits[3] || sd1->bits[4] || sd1->bits[5]) {
    Special_Decimal(decimal, 1 + sdgetbit(sd1, 223));
  } else {
    for (int i = 0; i < 3; i++) {
      decimal->bits[i] = sd1->bits[i];
    }
    decimal->bits[3] = sd1->bits[6];
  }
}

unsigned sdgetbit(s21_superdecimal* decimal, int bitn) {
  return (decimal->bits[bitn / 32] >> bitn % 32) % 2;
}

void sdsetbit(s21_superdecimal* decimal, int bitn, unsigned int value) {
  unsigned int dvalue = sdgetbit(decimal, bitn);
  if (dvalue != value) {
    if (value == 0) {
      decimal->bits[bitn / 32] -= (1 << bitn % 32);
    } else {
      decimal->bits[bitn / 32] += (1 << bitn % 32);
    }
  }
}

unsigned sdgetscale(s21_superdecimal* decimal) {
  return (decimal->bits[6] << 8) >> 24;
}

void sdBitSetValue(s21_superdecimal* decimal, unsigned value) {
  if (value == 1) value = -1;
  for (int l = 0; l < 6; l++) decimal->bits[l] = value;
}

void sdlshift(s21_superdecimal* decimal, int shift) {
  int i = 191 - shift;
  while (i >= 0) {
    unsigned int bit = sdgetbit(decimal, i);
    sdsetbit(decimal, (i + shift), bit);
    i--;
  }
  while (shift > 0) {
    sdsetbit(decimal, (shift - 1), 0);
    shift--;
  }
}

void sdrshift(s21_superdecimal* decimal, int shift) {
  int i = shift;
  while (i <= 191) {
    unsigned int bit = sdgetbit(decimal, i);
    sdsetbit(decimal, (i - shift), bit);
    i++;
  }
  while (shift > 0) {
    sdsetbit(decimal, (192 - shift), 0);
    shift--;
  }
}

unsigned sdbinarytouns(s21_superdecimal* decimal, unsigned bitn, unsigned num) {
  unsigned result = 0;
  if (num - 1 <= bitn) {
    unsigned mnoj = 1;
    for (; num > 0; num--) {
      result += sdgetbit(decimal, bitn - num + 1) * mnoj;
      mnoj *= 2;
    }
  }
  return result;
}

s21_superdecimal sdmul10(s21_superdecimal decimal) {
  s21_superdecimal decimal2 = decimal;
  sdlshift(&decimal2, 1);
  sdlshift(&decimal, 3);
  decimal = sdminisum(decimal, decimal2);
  return decimal;
}

s21_superdecimal sdminisum(s21_superdecimal s1, s21_superdecimal s2) {
  unsigned int buf = 0;
  s21_superdecimal res1 = s1;
  for (int l = 0; l < 6; l++) res1.bits[l] = 0;
  for (int i = 0; i < 192; i++) {
    unsigned int b1, b2, res;
    b1 = sdgetbit(&s1, i);
    b2 = sdgetbit(&s2, i);
    res = (b1 + b2 + buf) % 2;
    buf = (b1 + b2 + buf) / 2;
    sdsetbit(&res1, i, res);
  }
  if (buf) {
    s21_superdecimal one = {{1, 0, 0, 0, 0, 0, 0}, 0};
    res1 = sdminisum(res1, one);
  }
  return res1;
}

s21_superdecimal sddiv10(s21_superdecimal* decimal) {
  s21_superdecimal result = *decimal;
  sdBitSetValue(&result, 0);
  int i = sdGetFirstOne(decimal);
  unsigned g = 191;
  unsigned uns = sdbinarytouns(decimal, i, (i >= 3) ? 4 : i + 1);
  i -= (i >= 3) ? 4 : i + 1;
  if (uns >= 10) {
    uns -= 10;
    sdsetbit(&result, g--, 1);
  } else {
    sdsetbit(&result, g--, 0);
  }
  while (i >= 0) {
    if (uns < 10) {
      uns *= 2;
      uns += sdgetbit(decimal, i--);
    }
    if (uns >= 10) {
      uns -= 10;
      sdsetbit(&result, g--, 1);
    } else {
      sdsetbit(&result, g--, 0);
    }
  }
  sdrshift(&result, g + 1);
  return result;
}
void sdnorm(s21_superdecimal* decimal, s21_superdecimal* dec2) {
  unsigned s1 = sdgetscale(decimal);
  unsigned s2 = sdgetscale(dec2);
  if (s1 < s2) {
    while (s2 != s1) {
      *decimal = sdmul10(*decimal);
      decimal->bits[6] += 1 << 16;
      s1++;
    }
  } else if (s1 > s2) {
    while (s2 != s1) {
      *dec2 = sdmul10(*dec2);
      dec2->bits[6] += 1 << 16;
      s2++;
    }
  }
}
unsigned is_zero(s21_decimal* decimal) {
  return !(decimal->bits[0] || decimal->bits[1] || decimal->bits[2]);
}

s21_superdecimal sdinversion(s21_superdecimal decimal) {
  decimal.bits[0] = ~decimal.bits[0];
  decimal.bits[1] = ~decimal.bits[1];
  decimal.bits[2] = ~decimal.bits[2];
  decimal.bits[3] = ~decimal.bits[3];
  decimal.bits[4] = ~decimal.bits[4];
  decimal.bits[5] = ~decimal.bits[5];
  return decimal;
}

s21_superdecimal sdminisub(s21_superdecimal decimal, s21_superdecimal dst) {
  sdnorm(&decimal, &dst);
  s21_superdecimal invdecimal;
  if (!sd_is_equal(decimal, dst)) {
    invdecimal = sdinversion(dst);
    invdecimal = sdminisum(invdecimal, decimal);
  } else {
    sdBitSetValue(&invdecimal, 0);
  }
  return invdecimal;
}

int sd_is_equal(s21_superdecimal sd1, s21_superdecimal sd2) {
  return sd1.bits[0] == sd2.bits[0] && sd1.bits[1] == sd2.bits[1] &&
         sd1.bits[2] == sd2.bits[2] && sd1.bits[3] == sd2.bits[3] &&
         sd1.bits[4] == sd2.bits[4] && sd1.bits[5] == sd2.bits[5];
}

int sdFindBiggest(s21_superdecimal* sd1, s21_superdecimal* sd2) {
  int answer = 0;
  for (int i = 5; i >= 0; i--) {
    if (sd1->bits[i] > sd2->bits[i]) {
      break;
    }
    if (sd1->bits[i] < sd2->bits[i]) {
      answer = 1;
      break;
    }
  }
  return answer;
}

unsigned sdFirstDigit(s21_superdecimal* sd1) {
  int i = sdgetbit(sd1, 0);
  int cycle = 2;
  int l = 5;
  for (; l >= 0 && !sd1->bits[l]; l--) {
  }

  int bitn = (l + 1) * 32 - 1;
  while (!sdgetbit(sd1, bitn)) bitn--;
  for (int g = 1; g <= bitn; g++) {
    if (sdgetbit(sd1, g)) i += cycle;
    cycle *= 2;
    cycle = cycle % 10;
  }
  i = i % 10;
  return i;
}

unsigned FirstDigit(s21_decimal* s1) {
  int i = getbit(s1, 0);
  int cycle = 2;
  int l = 2;
  for (; l >= 0 && !s1->bits[l]; l--) {
  }
  int bitn = (l + 1) * 32 - 1;
  while (!getbit(s1, bitn)) bitn--;
  for (int g = 1; g <= bitn; g++) {
    if (getbit(s1, g)) i += cycle;
    cycle *= 2;
    cycle = cycle % 10;
  }
  i = i % 10;
  return i;
}

s21_superdecimal sdnormsum(s21_superdecimal s1, s21_superdecimal s2) {
  unsigned int buf = 0;
  s21_superdecimal res1 = s1;
  for (int l = 0; l < 6; l++) res1.bits[l] = 0;
  for (int i = 0; i < 192; i++) {
    unsigned int b1, b2, res;
    b1 = sdgetbit(&s1, i);
    b2 = sdgetbit(&s2, i);
    res = (b1 + b2 + buf) % 2;
    buf = (b1 + b2 + buf) / 2;
    sdsetbit(&res1, i, res);
  }
  return res1;
}

s21_superdecimal sdDopCode(s21_superdecimal dec1) {
  s21_superdecimal invdecimal;
  invdecimal = sdinversion(dec1);
  s21_superdecimal tmp = {{1, 0, 0, 0, 0, 0, 0}, 0};
  dec1 = sdnormsum(invdecimal, tmp);
  return dec1;
}

unsigned sdis_zero(s21_superdecimal* decimal) {
  return !(decimal->bits[0] || decimal->bits[1] || decimal->bits[2] ||
           decimal->bits[3] || decimal->bits[4] || decimal->bits[5]);
}
