#include "settings.h"

#include "perkManipulator.h"

namespace Settings {
	void ReadDescription(Json::Value& a_descriptionField) {
		auto* dataHandler = RE::TESDataHandler::GetSingleton();

		for (auto& field : a_descriptionField) {
			//Conditions
			auto& pluginsField = field["plugins"];
			int matchDegree = 0;
			if (pluginsField) {
				if (!pluginsField.isArray()) {
					_loggerError("Expected array in plugins field, got something else instead. Ignoring field.");
					continue;
				}

				bool valid = true;
				for (auto& pluginCondition : pluginsField) {
					if (!pluginCondition.isString()) {
						_loggerError("Expected string in plugins sub-field, got something else instead. Ignoring field");
						valid = false;

						if (!dataHandler->LookupModByName(pluginCondition.asString())) {
							valid = false;
						}
						matchDegree++;
					}
				}
				if (!valid) continue;
			}

			//Changes
			auto& changesField = field["changes"];
			if (changesField) {
				if (!changesField.isArray()) {
					_loggerError("Expected array in changes field, got something else instead. Ignoring field.");
					continue;
				}

				for (auto& change : changesField) {
					if (!change.isObject()) {
						_loggerError("Expected object in changes sub-field, got something else instead. Ignoring change.");
						continue;
					}

					auto& perkField = change["perk"];
					auto& descriptionField = change["newDescription"];
					if (!(perkField && descriptionField)) {
						_loggerError("Expected both <perk> and <newDescription>, but at least one is absent. Ignoring change.");
						continue;
					}
					if (!perkField.isString()) {
						_loggerError("Perk field is not a string when it should be, ignoring change.");
						continue;
					}
					if (!descriptionField.isString()) {
						_loggerError("Description field is not a string when it should be, ignoring change.");
						continue;
					}

					auto* perk = GetFormFromString<RE::BGSPerk>(perkField.asString());
					if (!perk) {
						_loggerInfo("Perk not found. This may not be an error. Perk ID: {}", perkField.asString());
						continue;
					}
					std::string newDescription = descriptionField.asString();
					if (newDescription.empty()) {
						_loggerError("Description error.");
						continue;
					}

					RE::BSString descriptionOut;
					perk->GetDescription(descriptionOut, perk);
					if (descriptionOut.empty()) {
						_loggerError("Perk {} has an empty description, or its description could not be resolved.", perkField.asString());
						continue;
					}

					auto response = PerkManipulation::Manipulator::ChangeDescription(newDescription, descriptionOut.c_str(), matchDegree);
					_loggerInfo("{}", response);
				}
			}
		}
	}

	void ReadAdditions(Json::Value& a_additionsField) {

	}

	bool Read() {
		std::vector<std::string> configPaths = std::vector<std::string>();
		try {
			configPaths = clib_util::distribution::get_configs(R"(Data\SKSE\Plugins\PerkAdjuster\)", "", ".json"sv);
		}
		catch (std::exception e) {
			_loggerError("Caught error {} while trying to fetch config files.", e.what());
			return false;
		}
		if (configPaths.empty()) return true;

		try {
			for (auto& config : configPaths) {
				std::ifstream rawJSON(config);
				Json::Reader  JSONReader;
				Json::Value   JSONFile;
				JSONReader.parse(rawJSON, JSONFile);

				auto& descriptionField = JSONFile["descriptions"];
				auto& additionsField = JSONFile["additions"];
				if (!(descriptionField || additionsField)) continue;

				if (additionsField) {
					if (!additionsField.isArray()) {
						_loggerError("Error: Expected addition array, got something else. Aborting.");
					}

					ReadAdditions(additionsField);
				}

				if (descriptionField) {
					if (!descriptionField.isArray()) {
						_loggerError("Error: Expected description array, got something else. Aborting.");
					}

					ReadDescription(descriptionField);
				}

			}
		}
		catch (Json::Exception e) {
			_loggerError("Caught error {} while reading configuration files.", e.what());
		}

		return true;
	}
}