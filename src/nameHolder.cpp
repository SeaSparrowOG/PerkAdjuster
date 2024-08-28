#include "nameHolder.h"

namespace {
	bool IsModPresent(std::string a_modName) {
		return RE::TESDataHandler::GetSingleton()->LookupModByName(a_modName) ? true : false;
	}

	bool IsHex(std::string const& s) {
		return s.compare(0, 2, "0x") == 0
			&& s.size() > 2
			&& s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
	}

	RE::BGSPerk* GetPerkFromText(std::string a_perkName) {
		if (const auto splitID = clib_util::string::split(a_perkName, "|"); splitID.size() == 2) {
			if (!IsHex(splitID[0])) return nullptr;
			const auto  formID = clib_util::string::to_num<RE::FormID>(splitID[0], true);

			const auto& modName = splitID[1];
			if (!IsModPresent(modName)) return nullptr;

			auto* baseForm = RE::TESDataHandler::GetSingleton()->LookupForm(formID, modName);
			auto*foundPerk = baseForm ? baseForm->As<RE::BGSPerk>() : nullptr;
			if (foundPerk) return foundPerk;
		}

		auto* form = RE::TESBoundObject::LookupByEditorID(a_perkName);
		if (form && form->As<RE::BGSPerk>()) return form->As<RE::BGSPerk>();
		return nullptr;
	}
}

namespace NameHolder {
	bool NameHolder::Install()
	{
		std::unordered_map<std::string, int> matches;
		std::vector<std::string> configPaths = std::vector<std::string>();
		try {
			configPaths = clib_util::distribution::get_configs(R"(Data\SKSE\Plugins\PerkRenamer\)", "", ".json"sv);
		}
		catch (std::exception e) {
			_loggerError("Caught error {} while trying to fetch fire config files.", e.what());
			return false;
		}
		if (configPaths.empty()) return true;

		for (auto& config : configPaths) {
			std::ifstream rawJSON(config);
			Json::Reader  JSONReader;
			Json::Value   JSONFile;
			JSONReader.parse(rawJSON, JSONFile);

			auto& rulesField = JSONFile["rules"];
			if (!rulesField || !rulesField.isArray()) continue;

			for (auto& rule : rulesField) {
				auto& conditionsField = rule["conditions"];
				auto& changesField = rule["changes"];
				if (!changesField || !changesField.isArray()) continue;
				if (conditionsField && !conditionsField.isObject()) continue;

				bool match = true;
				uint8_t matchDegree = 0;
				if (conditionsField) {
					auto& pluginConditions = conditionsField["plugins"];
					if (!pluginConditions || !pluginConditions.isArray()) continue;
				
					for (auto& pluginField : pluginConditions) {
						if (!pluginField.isString()) continue;
						if (!RE::TESDataHandler::GetSingleton()->LookupLoadedModByName(pluginField.asString())) {
							match = false;
							continue;
						}

						++matchDegree;
					}
				}
				if (!match) continue;

				for (auto& change : changesField) {
					auto& perkField = change["perk"];
					auto& descriptionField = change["description"];
					if (!perkField || !descriptionField) continue;
					if (!descriptionField.isString() || !perkField.isString()) continue;

					auto* perk = GetPerkFromText(perkField.asString());
					if (!perk) continue;

					RE::BSString oldDescription;
					perk->GetDescription(oldDescription, perk);
					if (oldDescription.empty()) continue;

					std::string newDescription = descriptionField.asString();
					if (newDescription.empty()) continue;

					if (matches.contains(newDescription) && matches[newDescription] > matchDegree) continue;

					matches[newDescription] = matchDegree;
					if (descriptionMatches.contains(oldDescription.c_str())) {
						descriptionMatches.erase(oldDescription.c_str());
					}
					descriptionMatches[oldDescription.c_str()] = newDescription;
					_loggerInfo("Created match: {} - {}", oldDescription.c_str(), descriptionMatches[oldDescription.c_str()]);
				}
			}
		}
		return true;
	}

	std::string NameHolder::GetNewDescription(std::string a_inString)
	{
		if (descriptionMatches.contains(a_inString)) {
			return descriptionMatches[a_inString];
		}
		return std::string();
	}
}