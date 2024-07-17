#pragma once

namespace NameHolder {
	struct ConditionalDescription {
		std::string a_newDesctription;
		uint8_t conditionMatches;
	};

	class NameHolder : public ISingleton<NameHolder> {
	public:
		bool        ReadSettings();
		bool        IsManagedPerk(RE::BGSPerk* a_perk);
		std::string GetNewDescription(RE::BGSPerk* a_perk);

	private:
		std::unordered_map<RE::BGSPerk*, ConditionalDescription> storedValues{};
	};
}