/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "complex_structs.h"


int struct_has_alphabet(struct One st) {
    for (int i = 0; i < 12; i++) {
        if (st.str[i] != (char) (65 + i)) {
            return 0;
        }
    }

    return 1;
}

char arrays_in_inner_structs(struct Three st) {
    if (st.b == 0) {
        return '0';
    }

    for (int i = 0; i < 2; i++) {
        if(st.chs[i] != 'c') {
            return '1';
        }
    }

    for (int i = 0; i < 5; i++) {
        if(st.in.ints[i] != 65) {
            return '2';
        }
    }

    return '3';

}


struct One alphabet(int a) {
    if (a < 0) {
        struct One res = {-1, {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'}};
        return res;
    }
    struct One res = {1, {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'}};
    return res;
}

struct RecursiveDoublePointer {
    struct RecursiveDoublePointer **next;
    struct RecursiveDoublePointer *previous[2];
    int value;
};

struct RecursiveDoublePointer recursiveDoublePointerGlobal;

struct RecursiveDoublePointer check_double_pointer(struct RecursiveDoublePointer recursiveDoublePointerParam) {
    recursiveDoublePointerParam.value = recursiveDoublePointerGlobal.value;
    return recursiveDoublePointerParam;
}

// Check that clang doesn't fail printing this structure while rewriting source to header
struct fetch_negotiator {
    struct object_id* (*next) (struct fetch_negotiator *x);
};
