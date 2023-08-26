#include <hooks.h>

namespace Hooks {

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

	RE::BGSPerk* GetPerkFromEditorID(std::string a_EDID) {
		RE::BGSPerk* defaultResponse = nullptr;

		auto perkArray = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSPerk>();

		for (auto it = perkArray.begin(); it != perkArray.end(); ++it) {

			RE::BGSPerk* candidatePerk = *it;
			std::string candidateEDID = GetFormEditorID(candidatePerk);

			if (candidateEDID == a_EDID) {

				return candidatePerk;
			}
		}
		return defaultResponse;
	}

	managedPerkData GeneratePerkData(RE::BGSPerk* a_perk, std::vector<managedPerkData> a_SwapData) {

		managedPerkData perkData;

		for (auto it = a_SwapData.begin(); it != a_SwapData.end(); ++it) {

			managedPerkData candidateData;
			candidateData = *it;

			if (a_perk == candidateData.managedPerk) {

				return candidateData;
			}
		}

		RE::BSString tempString;
		a_perk->GetDescription(tempString, a_perk);

		perkData.originalName = a_perk->GetName();
		perkData.EDID = GetFormEditorID(a_perk);
		perkData.managedPerk = a_perk;
		perkData.description = std::string(tempString.c_str());

		return perkData;
	}

	bool IsPerkManagedINI(RE::BGSPerk* a_perk, std::vector<managedPerkData> a_managedPerkData, std::string a_configName) {

		for (auto it = a_managedPerkData.begin(); it != a_managedPerkData.end(); ++it) {

			RE::BGSPerk* managedPerk;
			managedPerkData managedPerkData = *it;

			if (managedPerkData.managedPerk == a_perk) {

				//INI conflict.
				SKSE::log::warn("INI conflict for < {} >.", managedPerkData.EDID);
				SKSE::log::warn("Old INI holder : < {} > , new INI holder : < {} > .", managedPerkData.INIHandler, a_configName);
				return true;
			} 
		}

		return false;
	}

    bool DescriptionManager::AddManagedPerkINI(std::string a_originalPerk, std::string a_transplantPerk, std::string a_newName, std::string a_configName) {

		if (a_originalPerk.empty() || a_transplantPerk.empty() || a_newName.empty() || a_configName.empty()) {

			SKSE::log::error("Unhandled error while attempting to parse an INI file.");
			return false;
		}

		RE::BGSPerk* originalPerk;
		originalPerk = GetPerkFromEditorID(a_originalPerk);

		if (!originalPerk) {

			SKSE::log::warn("Invalid entry in {}, with EDID: {}.", a_configName, a_originalPerk);
			return false;
		}

		RE::BGSPerk* transplantPerk;
		transplantPerk = GetPerkFromEditorID(a_transplantPerk);

		if (!transplantPerk) {

			SKSE::log::warn("Invalid entry in {} - section {}, with EDID: {}.", a_configName, a_originalPerk, a_transplantPerk);
			return false;
		}

		RE::BSString tempString;
		originalPerk->GetDescription(tempString, originalPerk);

        managedPerkData newData;
		newData.originalName = originalPerk->GetName();
		newData.newName = a_newName;
		newData.EDID = GetFormEditorID(originalPerk);
		newData.managedPerk = originalPerk;
        newData.descriptionDonor = transplantPerk;
		newData.description = std::string(tempString.c_str());

		if (IsPerkManagedINI(originalPerk, SwapPerkData, a_configName)) {

			auto it = SwapPerkData.end();

			for (it; it != SwapPerkData.end(); ++it) {

				managedPerkData tempData;
				RE::BGSPerk* tempPerk;
				tempData = *it;
				tempPerk = tempData.managedPerk;

				if (tempPerk) {

					if (tempPerk == newData.managedPerk) {
						break;
					}
				}
			}

			SwapPerkData.erase(it);
		}

		newData.INIHandler = a_configName;

		SwapPerkData.push_back(newData);
        ManagedPerks.push_back(originalPerk);

		originalPerk->fullName = a_newName;
        return true;
    }

    bool DescriptionManager::AddManagedPerkPapyrus(RE::BGSPerk* a_originalPerk, RE::BGSPerk* a_transplantPerk, std::string a_newName) {

		bool bSuccess = false;

		if (!a_originalPerk || !a_transplantPerk || a_newName.empty()) {

			SKSE::log::error("Unhandled error while attempting to parse a Papyrus call.");
			return bSuccess;
		}

		bSuccess = true;

		managedPerkData newData;
		newData = GeneratePerkData(a_originalPerk, SwapPerkData);

		//it is possible that newData is not actually new, and the perk has been managed in the past.
		if (!newData.newName.empty() || !newData.newNamePapyrus.empty()) {

			auto it = SwapPerkData.begin();

			for (it; it != SwapPerkData.end(); ++it) {

				managedPerkData tempData;
				RE::BGSPerk* tempPerk;
				tempData = *it;
				tempPerk = tempData.managedPerk;

				if (tempPerk) {

					if (tempPerk == newData.managedPerk) {

						break;
					}
				}
			}

			SwapPerkData.erase(it);
		}

		RE::BSString tempString;
		a_originalPerk->GetDescription(tempString, a_originalPerk);
		a_originalPerk->fullName = a_newName;

		newData.runtimeDescriptionDonor = a_transplantPerk;
		newData.newNamePapyrus = a_newName;
		newData.description = std::string(tempString.c_str());

		SwapPerkData.push_back(newData);
		ManagedPerks.push_back(a_originalPerk);

        return bSuccess;
    }

    DescriptionManager* DescriptionManager::GetSingleton() {

        DescriptionManager singleton;
        return &singleton;
    }

    bool DescriptionManager::RemoveManagedPerkPapyrus(RE::BGSPerk* a_originalPerk) {

		/* TODO: Implement this. Should be useful later.
		managedPerkData newData;
		newData = GeneratePerkData(a_originalPerk, SwapPerkData);

		if (newData.runtimeDescriptionDonor) {

			auto it = std::find_if(SwapPerkData.begin(), SwapPerkData.end(), newData.runtimeDescriptionDonor == a_originalPerk);

			if (it != SwapPerkData.end()) {

				SwapPerkData.erase(it);
				return true;
			}
		}
		*/

        return false;
    }

    //=========================================
    //            Thunk Function
    //=========================================
    void DescriptionManager::thunk(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType) {

		if (a_this) {

			RE::BSString tempString;
			a_this->GetDescription(tempString, nullptr);

			for (auto it = SwapPerkData.begin(); it != SwapPerkData.end(); ++it) {

				managedPerkData tempData;
				tempData = *it;

				if (tempData.description == tempString.c_str()) {

					if (tempData.runtimeDescriptionDonor) {

						return func(tempData.runtimeDescriptionDonor->As<RE::TESDescription>(), a_out, tempData.runtimeDescriptionDonor, a_fieldType);
					}

					return func(tempData.descriptionDonor->As<RE::TESDescription>(), a_out, tempData.descriptionDonor, a_fieldType);
				}
			}
		}

		return func(a_this, a_out, a_parent, a_fieldType);
    }

	bool DescriptionManager::InstallHook() {

		write_thunk_call<DescriptionManager>(Hooks::Get_Description_Hook1.address());
		write_thunk_call<DescriptionManager>(Hooks::Get_Description_Hook2.address());
		write_thunk_call<DescriptionManager>(Hooks::Get_Description_Hook3.address());
		return true;
	}
}