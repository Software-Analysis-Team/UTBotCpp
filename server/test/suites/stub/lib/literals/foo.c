/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "foo.h"
#include "sum.h"
#include "mult.h"

int check_stubs(int a, int b) {
    if (sum(a, b) == 69) {
        return 1;
    }
    if (mult(a, b) == 322) {
        return 2;
    }
    if (mult(a, b) - sum(a, b) == 228) {
        return 3;
    }
    if (sum(a, b) == a + b) {
        return 4;
    } else {
        return 5;
    }
}