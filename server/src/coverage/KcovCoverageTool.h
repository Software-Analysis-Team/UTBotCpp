//
// Created by andrey on 05.06.2022.
//

#ifndef UNITTESTBOT_KCOVCOVERAGETOOL_H
#define UNITTESTBOT_KCOVCOVERAGETOOL_H

#include "CoverageAndResultsGenerator.h"
#include "CoverageTool.h"

class KcovCoverageTool : public CoverageTool {
public:
    KcovCoverageTool(utbot::ProjectContext projectContext, const ProgressWriter *progressWriter);

    std::vector<BuildRunCommand> getBuildRunCommands(const std::vector<UnitTest> &testsToLaunch,
                                                     bool withCoverage) override;

    std::vector<ShellExecTask> getCoverageCommands(const std::vector<UnitTest> &testFilePath) override;

    [[nodiscard]] Coverage::CoverageMap getCoverageInfo() const override;
    [[nodiscard]] nlohmann::json getTotals() const override;
    void cleanCoverage() const override;
private:
    const utbot::ProjectContext projectContext;
};


#endif //UNITTESTBOT_KCOVCOVERAGETOOL_H
