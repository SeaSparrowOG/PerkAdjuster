#include "iniParser.h"

namespace iniParser {

	//PO3, relinquish this to me.
	static std::string GetFormEditorID(const RE::TESForm* a_form)
	{
		switch (a_form->GetFormType()) {
		case RE::FormType::Keyword:
		case RE::FormType::LocationRefType:
		case RE::FormType::Action:
		case RE::FormType::MenuIcon:
		case RE::FormType::Global:
		case RE::FormType::HeadPart:
		case RE::FormType::Race:
		case RE::FormType::Sound:
		case RE::FormType::Script:
		case RE::FormType::Navigation:
		case RE::FormType::Cell:
		case RE::FormType::WorldSpace:
		case RE::FormType::Land:
		case RE::FormType::NavMesh:
		case RE::FormType::Dialogue:
		case RE::FormType::Quest:
		case RE::FormType::Idle:
		case RE::FormType::AnimatedObject:
		case RE::FormType::ImageAdapter:
		case RE::FormType::VoiceType:
		case RE::FormType::Ragdoll:
		case RE::FormType::DefaultObject:
		case RE::FormType::MusicType:
		case RE::FormType::StoryManagerBranchNode:
		case RE::FormType::StoryManagerQuestNode:
		case RE::FormType::StoryManagerEventNode:
		case RE::FormType::SoundRecord:
			return a_form->GetFormEditorID();
		default:
		{
			static auto tweaks = GetModuleHandle(L"po3_Tweaks");
			if (tweaks) {
				static auto func = reinterpret_cast<const char* (*)(std::uint32_t)>(GetProcAddress(tweaks, "GetFormEditorID"));
				if (func) {
					return func(a_form->formID);
				}
			}
			return {};
		}
		}
	}

	RE::BGSPerk* FindPerkInArray(RE::BSTArray<RE::BGSPerk*, RE::BSTArrayHeapAllocator> a_perkArray, const char* a_name) {

		RE::BGSPerk* fail = nullptr;

		for (auto iterator = a_perkArray.begin(); iterator != a_perkArray.end(); ++iterator) {

			RE::BGSPerk* perk = *iterator;

			if (!perk) {

				continue;
			}

			auto edid = GetFormEditorID(perk);

			if (edid == a_name) {
				
				return perk;
			}
		}

		return fail;
	}

	std::vector<CSimpleIniA::Entry> RetrieveAndManipulate() {

		std::vector<CSimpleIniA::Entry> result;
		auto directory = R"(Data\)";
		auto suffix = "_PRKR"sv;
		auto data_handler = RE::TESDataHandler::GetSingleton();
		auto perk_array = data_handler->GetFormArray<RE::BGSPerk>();

		std::vector<std::string> paths = clib_util::distribution::get_configs(directory, suffix);
		
		for (const auto& foundPath : paths) {

			CSimpleIniA ini;
			ini.SetUnicode();
			ini.SetMultiKey();

			if (const auto rc = ini.LoadFile(foundPath.c_str()); rc < 0) {

				SKSE::log::info("Failed to find path.");
				continue;
			}

			std::list<CSimpleIniA::Entry> entries;
			ini.GetAllSections(entries); 

			for (auto& localEntry : entries) {

				auto entry_name = localEntry.pItem;
				RE::BGSPerk* associatedForm;

				associatedForm = FindPerkInArray(perk_array, entry_name); 

				if (!associatedForm) {

					SKSE::log::info("Found entry {} with no associated form.", entry_name);
					continue;
				}

				auto transplantPerkID = ini.GetValue(entry_name, "description");

				if(!transplantPerkID) {

					SKSE::log::info("Ini entry {} is invalid.", entry_name);
					continue;
				}

				RE::BGSPerk* newDescriptionHolder; 

				newDescriptionHolder = FindPerkInArray(perk_array, transplantPerkID);

				if (!newDescriptionHolder) {

					SKSE::log::info("Donor perk entry {} with no associated form.", transplantPerkID);
					continue;
				}

				auto new_name = ini.GetValue(entry_name, "name");

				if (new_name == 0) {

					SKSE::log::info("Entry {} contains invalid data.", entry_name);
					continue;
				}
				
				if (!manipulate::AdjustPerk(associatedForm, new_name, newDescriptionHolder)){

					SKSE::log::info("Entry {} could not be manipulated.", entry_name);
					continue;
				}

				result.push_back(localEntry);
			}
		}
		
		return result;
	}
}