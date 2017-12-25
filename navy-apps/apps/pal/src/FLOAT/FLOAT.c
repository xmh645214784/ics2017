#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

static union {
  struct {
   unsigned m : 23;
    unsigned e : 8;
    unsigned s : 1;
  } split;
  struct {
    unsigned f : 16;
    unsigned i : 15;
    unsigned s : 1;
  } split_u;
  unsigned u;
} tmp_float;

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // assert(0);
  unsigned rega, regb;
  asm volatile ("imul %2" : "=a"(rega), "=d"(regb) : "r"(b), "a"(a));
  return (rega >> 16) + (regb << 16);
  // return 0;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  // assert(0);
  unsigned rega = a << 16, regb = (unsigned)a >> 16, q;
  FLOAT result;
  asm volatile ("idiv %2" : "=a"(result), "=d"(q) : "r"(b), "a"(rega), "d"(regb));
  return result;
  // return 0;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  unsigned *p_a=(unsigned*)&a;
  tmp_float.u=*p_a;
  if (tmp_float.split.e==0){
    return 0;
  }
  else if(tmp_float.split.e==0xff){
    return tmp_float.split.s ? -0x7fffffff : 0x7fffffff;
  }
  else {
    int e = (int)tmp_float.split.e - 134;
    unsigned m = (tmp_float.split.m | 0x800000);
    int sum = (e >= 0) ? (m << e) : (m >> -e);
    return  (tmp_float.split.s ? -sum : sum );
  }
  // assert(0);
  // return 0;
}

FLOAT Fabs(FLOAT a) {
  // assert(0);
  tmp_float.u = (unsigned)a;
  return (tmp_float.split_u.s) ? -a : a;
  // return 0;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
