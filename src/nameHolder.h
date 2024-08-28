#pragma once

namespace NameHolder {
	class NameHolder : public ISingleton<NameHolder> {
	public:
		bool        Install();
		std::string GetNewDescription(std::string a_inString);

	private:
		std::unordered_map<std::string, std::string> descriptionMatches;
	};
}