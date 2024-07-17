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
	bool NameHolder::IsManagedPerk(RE::BGSPerk* a_perk)
	{
		return storedValues.contains(a_perk);
	}

	std::string NameHolder::GetNewDescription(RE::BGSPerk* a_perk)
	{
		const char* response = "";
		if (!storedValues.contains(a_perk)) return response;
		return storedValues[a_perk].a_newDesctription;
	}

	bool NameHolder::ReadSettings()
	{
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
				auto& descriptionField = rule["description"];
				auto& perkField = rule["perk"];
				auto& conditionsField = rule["conditions"];
				if (!descriptionField || !perkField) continue;
				if (!descriptionField.isString() || !perkField.isString()) continue;
				if (conditionsField && !conditionsField.isObject()) continue;

				auto* perk = GetPerkFromText(perkField.asString());
				if (!perk) continue;
				std::string newDescription = descriptionField.asString();
				if (newDescription.empty()) continue;

				bool match = true;
				uint8_t matchDegree = 0;
				if (conditionsField) {
					auto& pluginConditions = conditionsField["plugins"];
					if (!pluginConditions || !pluginConditions.isArray());
				
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
				if (IsManagedPerk(perk) && (matchDegree < storedValues[perk].conditionMatches)) continue;

				auto newData = ConditionalDescription();
				newData.conditionMatches = matchDegree;
				newData.a_newDesctription = newDescription;
				storedValues[perk] = newData;
			}
		}
		return true;
	}
}