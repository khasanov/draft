#include "source_manager.h"

namespace draft {

Source::Id SourceManager::makeSource(const std::string &source, const std::string &path)
{
    Source::Id id = sources.size();
    paths.emplace_back(path);
    sources.emplace_back(source);
    return id;
}

const std::string &SourceManager::getPath(Source::Id id) const
{
    return paths.at(id);
}

const Source &SourceManager::getSource(Source::Id id) const
{
    return sources.at(id);
}

}  // namespace draft
