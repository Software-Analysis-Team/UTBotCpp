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
#include "pugixml.hpp"
#include <regex>

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
                result.push_back({testToLaunch, buildCommand, runCommand});
            });
    return result;
}

std::vector<ShellExecTask>
KcovCoverageTool::getCoverageCommands(const std::vector<UnitTest> &testsToLaunch) {
    return {};
}


static void addLine(uint32_t lineNumber, bool covered, FileCoverage &fileCoverage) {
    assert(lineNumber > 0);
    if (covered) {
        fileCoverage.fullCoverageLines.insert({lineNumber});
    } else {
        fileCoverage.noCoverageLines.insert({lineNumber});
    }
}

static void setLineNumbers(pugi::xml_node lines, FileCoverage &fileCoverage) {
    for (pugi::xml_node line: lines.children("line")) {
        uint32_t lineNumber = line.attribute("number").as_int();
        bool covered = line.attribute("hits").as_int() > 0;
        addLine(lineNumber, covered, fileCoverage);
    }
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


    std::string pathToReport = "";
    for (const auto &entry: fs::directory_iterator(coverageReportDirPath.string())) {
        std::regex reg("(calc.).*$");
        if (std::regex_search(entry.path().string(), reg)) {
            pathToReport = entry.path().string() + "/cov.xml";
            break;
        }
    }

    auto path = pathToReport.c_str();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path);
    auto tools = doc.child("coverage").child("packages").child("package").child("classes");


    for (pugi::xml_node fileIter = tools.child("class"); fileIter; fileIter = fileIter.next_sibling("class")) {
        std::string filePathString = fileIter.attribute("filename").as_string();
        fs::path filePath(filePathString);

        pugi::xml_node lines = fileIter.child("lines");

        setLineNumbers(lines, coverageMap[filePath]);
        //setFunctionBorders(lines, coverageMap[filePath]);
    }

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