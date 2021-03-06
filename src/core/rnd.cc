/*	SCCS Id: @(#)rnd.c	3.4	1996/02/07	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "core/hack.h"

/* "Rand()"s definition is determined by [OS]conf.h */
#if defined(LINT) /* rand() is long... */
extern int rand();
#define RND(x) (rand() % x)
#else /* LINT */
#define RND(x) (int)(Rand() % (long)(x))
#endif /* LINT */


static int reseed_period = 0;
static int reseed_count = 0;

void check_reseed() {
  reseed_count++;
  if (reseed_count > reseed_period) {
    FILE *fptr = NULL;
    int rnd[2];

    fptr = fopen("/dev/urandom", "r");
    if (fptr)
      fread((void *)rnd, sizeof(int), 2, fptr);
    fclose(fptr);
    srandom((int)(time(nullptr)) + rnd[0]);
    reseed_period = (rnd[1] % 700) + 10;
    reseed_count = 0;
  }
}

/* 0 <= rn2(x) < x */
int rn2(int x) {
  check_reseed();
#ifdef DEBUG
  if (x <= 0) {
    impossible("rn2(%d) attempted", x);
    return (0);
  }
  x = RND(x);
  return (x);
#else
  return (RND(x));
#endif
}


/* 0 <= rnl(x) < x; sometimes subtracting Luck */
/* good luck approaches 0, bad luck approaches (x-1) */
int rnl(int x) {
  int i;
  check_reseed();
#ifdef DEBUG
  if (x <= 0) {
    impossible("rnl(%d) attempted", x);
    return (0);
  }
#endif
  i = RND(x);

  if (Luck && rn2(50 - Luck)) {
    i -= (x <= 15 && Luck >= -5 ? Luck / 3 : Luck);
    if (i < 0)
      i = 0;
    else if (i >= x)
      i = x - 1;
  }

  return i;
}


/* 1 <= rnd(x) <= x */
int rnd(int x) {
  check_reseed();
#ifdef DEBUG
  if (x <= 0) {
    impossible("rnd(%d) attempted", x);
    return (1);
  }
  x = RND(x) + 1;
  return (x);
#else
  return (RND(x) + 1);
#endif
}


/* n <= d(n,x) <= (n*x) */
int d(int n, int x) {
  int tmp = n;
  check_reseed();

#ifdef DEBUG
  if (x < 0 || n < 0 || (x == 0 && n != 0)) {
    impossible("d(%d,%d) attempted", n, x);
    return (1);
  }
#endif
  while (n--)
    tmp += RND(x);
  return (tmp); /* Alea iacta est. -- J.C. */
}


int rne(int x) {
  int tmp, utmp;

  utmp = (player.ulevel < 15) ? 5 : player.ulevel / 3;
  tmp = 1;
  while (tmp < utmp && !rn2(x))
    tmp++;
  return tmp;

  /* was:
   *	tmp = 1;
   *	while(!rn2(x)) tmp++;
   *	return(min(tmp,(player.ulevel < 15) ? 5 : player.ulevel/3));
   * which is clearer but less efficient and stands a vanishingly
   * small chance of overflowing tmp
   */
}

int rnz(int i) {
#ifdef LINT
  int x = i;
  int tmp = 1000;
#else
  long x = i;
  long tmp = 1000;
#endif
  tmp += rn2(1000);
  tmp *= rne(4);
  if (rn2(2)) {
    x *= tmp;
    x /= 1000;
  } else {
    x *= 1000;
    x /= tmp;
  }
  return ((int)x);
}


/*rnd.c*/
