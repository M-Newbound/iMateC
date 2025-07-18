/* iMate -- Copyright (C) 2024 Martin Newbound
 *
 * Minimal single-header test framework.
 *
 * Usage:
 *   CHECK(expr);
 *   CHECK_EQ(a, b);
 *   RESULT();  // at end of main(), prints pass/fail and returns exit code
 */

#pragma once

#include <stdio.h>
#include <math.h>

static int _pass = 0;
static int _fail = 0;

#define CHECK(cond) do {                                                    \
    if (cond) {                                                             \
        _pass++;                                                            \
    } else {                                                                \
        _fail++;                                                            \
        fprintf(stderr, "  FAIL  %s:%d  (%s)\n",                           \
                __FILE__, __LINE__, #cond);                                 \
    }                                                                       \
} while (0)

#define CHECK_EQ(a, b)          CHECK((a) == (b))
#define CHECK_NE(a, b)          CHECK((a) != (b))
#define CHECK_TRUE(x)           CHECK(!!(x))
#define CHECK_FALSE(x)          CHECK(!(x))
#define CHECK_NULL(x)           CHECK((x) == NULL)
#define CHECK_NOT_NULL(x)       CHECK((x) != NULL)

#define CHECK_FLOAT_NEAR(a, b, eps) \
    CHECK(fabsf((float)(a) - (float)(b)) < (float)(eps))

#define RESULT() do {                                                       \
    int _total = _pass + _fail;                                             \
    if (_fail == 0) {                                                       \
        printf("OK  %d/%d passed\n", _pass, _total);                       \
    } else {                                                                \
        printf("FAIL  %d/%d passed  (%d failed)\n",                        \
               _pass, _total, _fail);                                       \
    }                                                                       \
    return _fail ? 1 : 0;                                                   \
} while (0)
