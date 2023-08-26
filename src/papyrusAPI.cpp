#include <papyrusAPI.h>
#include <hooks.h>

namespace PapyrusAPI {

	std::vector<int32_t> GetRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*) {

		return { 1, 0, 0 };
	}

	bool ManipulatePerk(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BSString a_newName, RE::BGSPerk* a_transplantPerk) {

		if (!a_perk || !a_transplantPerk || a_newName.empty()) {

			SKSE::log::info("Exception thrown by Papyrus function.");
			return false;
		}

		auto manager = Hooks::DescriptionManager::GetSingleton();

		if (!manager) {

			SKSE::log::info("Exception thrown by the PapyrusAPI. Could not get the manager singleton.");
			return false;
		}

		std::string newName = std::string(a_newName.c_str());
		return manager->AddManagedPerkPapyrus(a_perk, a_transplantPerk, newName);
	}

	bool Bind(VM* a_vm) {

		BIND(ManipulatePerk);
		BIND(GetRenamerVersion);
		SKSE::log::info("Bound Perk Renamer functions.");
		return true;
	}
}