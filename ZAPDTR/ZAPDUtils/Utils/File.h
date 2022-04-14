#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace File
{

bool Exists(const std::filesystem::path& filePath);

std::string ReadAllText(const std::filesystem::path& filePath);

std::vector<std::string> ReadAllLines(const std::filesystem::path& filePath);

void WriteAllText(const std::filesystem::path& filePath, const std::string& text);

}
