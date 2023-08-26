#include <hooks.h>

namespace Hooks {

	//=========================================
	//            Helper Functions
	//=========================================

	managedPerkData GeneratePerkData(RE::BGSPerk* a_perk, RE::BGSPerk* a_transplantPerk, std::string a_newName, 
		bool a_bManagedByPapyrus, bool a_bManagedByINI, std::string a_managerConfig) {

		managedPerkData newData;

		newData.managedByINI = a_bManagedByINI;
		newData.managedByPapyrus = a_bManagedByPapyrus;
		newData.originalName = a_perk->GetName();
		
		if (a_bManagedByINI) {

			newData.newName = a_newName;
			newData.descriptionDonor = a_transplantPerk;
			newData.INIHandler = a_managerConfig;
		}
		else {

			newData.newNamePapyrus = a_newName;
			newData.runtimeDescriptionDonor = a_transplantPerk;
		}

		newData.managedPerk = a_perk;

		RE::BSString descriptionHolder;
		a_perk->GetDescription(descriptionHolder, a_perk);
		newData.description = std::string(descriptionHolder.c_str());

		return newData;
	}

	bool IsPerkManagedINI(RE::BGSPerk* a_perk, std::vector<managedPerkData> a_managedPerkData, std::string a_configName) {

		for (auto it = a_managedPerkData.begin(); it != a_managedPerkData.end(); ++it) {

			RE::BGSPerk* managedPerk;
			managedPerkData perkData = *it;

			if (perkData.managedPerk == a_perk) {

				if (a_configName.empty()) {

					//Perk managed by INI.
					SKSE::log::info("Papyrus API call to edit < {} > that is already managed by config < {} >.", a_perk->GetName(), perkData.INIHandler);
				}
				else {

					//INI conflict.
					SKSE::log::warn("INI conflict for < {} >.", a_perk->GetName());
					SKSE::log::warn("Old INI holder : < {} > , new INI holder : < {} > .", perkData.INIHandler, a_configName);
				}

				return true;
			} 
		}

		return false;
	}

	bool IsPerkManagedPapyrus(managedPerkData a_data, std::vector<managedPerkData> a_managedPerkData) {

		for (auto it = a_managedPerkData.begin(); it != a_managedPerkData.end(); ++it) {

			managedPerkData tempData;
			tempData = *it;

			if (tempData.managedPerk == a_data.managedPerk) {

				SKSE::log::info("Perk < {} > is already managed by Papyrus. Previous definition will be removed.", tempData.originalName);
				return true;
			}
		}

		return false;
	}

	//=========================================
	//            Class Functions
	//=========================================

	DescriptionManager* DescriptionManager::GetSingleton() {

		DescriptionManager singleton;
		return &singleton;
	}

    bool DescriptionManager::AddManagedPerkINI(std::string a_originalPerk, std::string a_transplantPerk, std::string a_newName, std::string a_configName) {

		if (a_originalPerk.empty() || a_transplantPerk.empty() || a_newName.empty() || a_configName.empty()) {

			SKSE::log::error("Unhandled error while attempting to parse an INI file.");
			return false;
		}

		RE::BGSPerk* originalPerk = RE::TESForm::LookupByEditorID<RE::BGSPerk>(a_originalPerk);
		RE::BGSPerk* transplantPerk = RE::TESForm::LookupByEditorID<RE::BGSPerk>(a_transplantPerk);

		if (!originalPerk) {

			SKSE::log::info("Perks not found for swap [ {} ] in INI < {} >.", a_originalPerk, a_configName);
			return false;
		}

		if (!transplantPerk) {

			SKSE::log::info("Failed to find perk description < {} > for swap [ {} ] in INI < {} >.",a_transplantPerk, a_originalPerk, a_configName);
			return false;
		}

        managedPerkData newData;
		newData = GeneratePerkData(originalPerk, transplantPerk, a_newName, false, true, a_configName);

		if (IsPerkManagedINI(originalPerk, SwapPerkData, a_configName)) {

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
		newData = GeneratePerkData(a_originalPerk, a_transplantPerk, a_newName, true, false, std::string());

		//it is possible that newData is not actually new, and the perk has been managed in the past.
		if (IsPerkManagedPapyrus(newData, SwapPerkData) || IsPerkManagedINI(a_originalPerk, SwapPerkData, std::string())) {

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

		SwapPerkData.push_back(newData);
		ManagedPerks.push_back(a_originalPerk);

        return bSuccess;
    }

    bool DescriptionManager::RemoveManagedPerkPapyrus(RE::BGSPerk* a_originalPerk) {

		//TODO: Implement this. Should be useful later.
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

					if (!tempData.newNamePapyrus.empty()) {

						return func(tempData.runtimeDescriptionDonor->As<RE::TESDescription>(), a_out, tempData.runtimeDescriptionDonor, a_fieldType);
					}

					return func(tempData.descriptionDonor->As<RE::TESDescription>(), a_out, tempData.descriptionDonor, a_fieldType);
				}
			}
		}

		return func(a_this, a_out, a_parent, a_fieldType);
    }

	bool DescriptionManager::InstallHook() {

		//Seems to be the only hook needed. If stuff disappears, I will add the other 2 in.
		write_thunk_call<DescriptionManager>(Hooks::Get_Description_Hook.address());
		return true;
	}
}