/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "foo.h"

static int exit_code(int argc) {
    if (argc == 1) {
        return 0;
    }
    return 1;
}

int main(/*int argc, char** argv*/) {
    return exit_code(0);
}