//
// Created by andrey on 05.06.2022.
//

#include "KcovCoverageTool.h"

#include "Coverage.h"
#include "Paths.h"
#include "TimeExecStatistics.h"
#include "environment/EnvironmentPaths.h"
#include "exceptions/CoverageGenerationException.h"
#include "utils/ArgumentsUtils.h"
#include "utils/CollectionUtils.h"
#include "utils/FileSystemUtils.h"
#include "utils/JsonUtils.h"
#include "utils/MakefileUtils.h"
#include "utils/StringUtils.h"
#include "utils/path/FileSystemPath.h"

#include "loguru.h"

using Coverage::CoverageMap;
using Coverage::FileCoverage;


KcovCoverageTool::KcovCoverageTool(utbot::ProjectContext projectContext,
                                   ProgressWriter const *progressWriter)
        : CoverageTool(progressWriter), projectContext(projectContext) {
}

std::vector<BuildRunCommand>
KcovCoverageTool::getBuildRunCommands(const std::vector<UnitTest> &testsToLaunch, bool withCoverage) {
    return {};
}

std::vector<ShellExecTask>
KcovCoverageTool::getCoverageCommands(const std::vector<UnitTest> &testsToLaunch) {
    return {};
}

Coverage::CoverageMap KcovCoverageTool::getCoverageInfo() const {
    return {};
}



