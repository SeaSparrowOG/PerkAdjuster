#include "settings.h"

#include "perkManipulator.h"

namespace Settings {
	static inline void FindNode(Json::Value& a_perkField,
		std::vector<RE::BGSSkillPerkTreeNode*>& a_result,
		RE::ActorValueInfo* a_tree) {
		for (auto& field : a_perkField) {
			if (!field.isString()) continue;
			auto* perk = GetFormFromString<RE::BGSPerk>(field.asString());
			if (!perk) continue;

			auto itStart = a_tree->perkTree->children.begin();
			auto itEnd = a_tree->perkTree->children.end();
			bool found = false;
			for (; !found && itStart != itEnd; ++itStart) {
				auto* node = *itStart;
				auto* treePerk = node->perk;
				if (treePerk != perk) continue;

				found = true;
				a_result.push_back(*itStart);
			}

			if (!found) {
				//TODO: Figure out a proper way to handle perks that WILL be added to the perk tree.
				_loggerError("Perk {} was found, but was not in the tree.", field.asString());
			}
		}
	}

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
		auto* dataHandler = RE::TESDataHandler::GetSingleton();

		for (auto& field : a_additionsField) {
			auto& perkField = field["perk"];
			auto& skillField = field["skill"];
			auto& xField = field["x"];
			auto& yField = field["y"];
			auto& parentField = field["parents"];
			auto& childField = field["children"];

			if (!(perkField && skillField && xField && yField)) {
				_loggerError("Missing at least one field in the perk addition section. Section won't be read.");
				continue;
			}

			if (!perkField.isString() || !skillField.isString()) {
				_loggerError("At least one string field in the perk addition section is not a string. Section won't be read.");
				continue;
			}

			if (!xField.isDouble() || !yField.isDouble()) {
				_loggerError("At least one float field in the perk addition section is not a float. Section won't be read.");
				continue;
			}

			if (parentField && !parentField.isArray()) {
				_loggerError("Parents field is not an array. Section won't be read.");
				continue;
			}

			if (childField && !childField.isArray()) {
				_loggerError("Children field is not an array. Section won't be read.");
				continue;
			}

			auto* perk = GetFormFromString<RE::BGSPerk>(perkField.asString());
			auto* avif = GetFormFromString<RE::ActorValueInfo>(skillField.asString());
			if (!avif || !perk) {
				_loggerError("Actor value or perk not found. Section won't be read. Strings: {} // {}", skillField.asString(), perkField.asString());
				continue;
			}
			float x = xField.asFloat();
			float y = yField.asFloat();

			std::vector<RE::BGSSkillPerkTreeNode*> parents{};
			std::vector<RE::BGSSkillPerkTreeNode*> children{};

			if (parentField) { FindNode(parentField, parents, avif); }
			if (childField) { FindNode(childField, children, avif); }

			PerkManipulation::Manipulator::PlaceNewPerk(perk, avif, x, y, parents, children);
		}
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