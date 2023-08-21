#include <papyrusAPI.h>
#include <perkManipulator.h>

namespace papyrusAPI {

	bool Bind(VM* a_vm) {
		
		BIND(PerkReassignment);
		BIND(GetRenamerVersion);
		SKSE::log::info("Bound Perk Renamer functions.");
		return true;
	}

	std::vector<int32_t> GetRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*) {

		return { 1, 0, 0 };
	}

	bool PerkReassignment(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BSString a_newName, RE::BGSPerk* a_transplantPerk) {

		if (!a_perk || !a_transplantPerk) {

			return false;
		}

		manipulate::AdjustPerk(a_perk, a_newName.c_str(), a_transplantPerk);
		return true;
	}
}