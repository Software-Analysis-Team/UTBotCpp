/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#ifndef UNITTESTBOT_STUBGEN_H
#define UNITTESTBOT_STUBGEN_H

#include "KleeGenerator.h"
#include "testgens/BaseTestGen.h"
#include "utils/TimeUtils.h"

#include "utils/path/FileSystemPath.h"
#include <unordered_map>
#include <vector>

class StubGen {
public:
    explicit StubGen(BaseTestGen &testGen);

    CollectionUtils::FileSet getStubSources(const fs::path &target);

    CollectionUtils::FileSet
    findStubFilesBySignatures(const std::vector<tests::Tests::MethodDescription> &signatures);

    static tests::Tests mergeSourceFileIntoStub(const tests::Tests &methodDescription,
                                                const tests::Tests &srcFile);

private:
    BaseTestGen &testGen;

    static bool cmpMethodsDecl(const tests::Tests::MethodDescription &decl1,
                               const tests::Tests::MethodDescription &decl2);
};


#endif // UNITTESTBOT_STUBGEN_H
