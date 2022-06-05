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
//#include "pugixml.hpp"

#include "loguru.h"

using Coverage::CoverageMap;
using Coverage::FileCoverage;


KcovCoverageTool::KcovCoverageTool(utbot::ProjectContext projectContext,
                                   ProgressWriter const *progressWriter)
        : CoverageTool(progressWriter), projectContext(projectContext) {
}

std::vector<BuildRunCommand>
KcovCoverageTool::getBuildRunCommands(const std::vector<UnitTest> &testsToLaunch, bool withCoverage) {
    ExecUtils::throwIfCancelled();

    std::vector<BuildRunCommand> result;
    ExecUtils::doWorkWithProgress(
            testsToLaunch, progressWriter, "Collecting build and run commands",
            [&](UnitTest const &testToLaunch) {
                auto makefile = Paths::getMakefilePathFromSourceFilePath(
                        projectContext,
                        Paths::testPathToSourcePath(projectContext, testToLaunch.testFilePath));
                auto gtestFlags = getTestFilter(testToLaunch);
                auto buildCommand =
                        MakefileUtils::makefileCommand(projectContext, makefile, "build", gtestFlags);
                auto runCommand =
                        MakefileUtils::makefileCommand(projectContext, makefile, "run", gtestFlags);
                result.push_back({ testToLaunch, buildCommand, runCommand });
            });
    return result;
}

std::vector<ShellExecTask>
KcovCoverageTool::getCoverageCommands(const std::vector<UnitTest> &testsToLaunch) {
    fs::path kcovFile = getKcovReportFile();

}

Coverage::CoverageMap KcovCoverageTool::getCoverageInfo() const {
    ExecUtils::throwIfCancelled();

    CoverageMap coverageMap;

    auto coverageReportDirPath = Paths::getKcovReportDir(projectContext);
    if (!fs::exists(coverageReportDirPath)) {
        std::string message = "Couldn't find coverage directory at " + coverageReportDirPath.string();
        LOG_S(ERROR) << message;
        throw CoverageGenerationException(message);
    }
    LOG_S(INFO) << "Reading coverage file";

//    ExecUtils::doWorkWithProgress(
//            FileSystemUtils::DirectoryIterator(covJsonDirPath), progressWriter,
//            "Reading coverage files", [&coverageMap](auto const &entry) {
//                auto jsonPath = entry.path();
//                auto coverageJson = JsonUtils::getJsonFromFile(jsonPath);
//                for (const nlohmann::json &jsonFile : coverageJson.at("files")) {
//                    fs::path filePath(std::filesystem::path(jsonFile.at("file")));
//                    if (Paths::isGtest(filePath)) {
//                        continue;
//                    }
//                    setLineNumbers(jsonFile, coverageMap[filePath]);
//                    setFunctionBorders(jsonFile, coverageMap[filePath]);
//                }
//            });
    return coverageMap;
}











nlohmann::json KcovCoverageTool::getTotals() const {
    return nlohmann::json();
}

void KcovCoverageTool::cleanCoverage() const {

}

fs::path KcovCoverageTool::getKcovReportFile() const {
    return nullptr;
}



