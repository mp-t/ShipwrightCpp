#include "Utils/File.h"
#include "Utils/StringHelper.h"

#include <fstream>

namespace File
{

bool Exists(const std::filesystem::path& filePath)
{
	return std::filesystem::exists(filePath);
}

std::string ReadAllText(const std::filesystem::path& filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
	const auto fileSize = file.tellg();
	file.seekg(0);

	std::string str(static_cast<std::size_t>(fileSize), '\0');
	file.read(str.data(), fileSize);	

	return str;
}

std::vector<std::string> ReadAllLines(const std::filesystem::path& filePath)
{
	std::string text = ReadAllText(filePath);
	return StringHelper::Split(text, "\n");
}

void WriteAllText(const std::filesystem::path& filePath, const std::string& text)
{
	std::ofstream file(filePath, std::ios::out);
	file.write(text.c_str(), text.size());
}

}