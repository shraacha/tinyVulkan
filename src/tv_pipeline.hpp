#pragma once

#include <string>
#include <vector>

namespace tv {
class TvPipeline {
public:
  TvPipeline(const std::string &vertFilepath, const std::string &fragFilepath);

private:
  static std::vector<char> readFile(const std::string &filepath);
  void createGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath);
};
} // namespace tv
