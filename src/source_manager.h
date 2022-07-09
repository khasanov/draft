#pragma once

#include <vector>

#include "source.h"

namespace draft {

class SourceManager {
public:
    Source::Id makeSource(const std::string &source, const std::string &path = "");

    const std::string &getPath(Source::Id id) const;
    const Source &getSource(Source::Id id) const;

private:
    std::vector<std::string> paths;
    std::vector<Source> sources;
};

}  // namespace draft
