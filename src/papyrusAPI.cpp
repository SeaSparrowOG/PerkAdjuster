#include <papyrusAPI.h>

namespace papyrusAPI {

	bool Bind(VM* a_vm) {
		
		//BIND(ReevaluatePerk);
		//BIND(GetPerkRenamerVersion);
		SKSE::log::info("Bound Perk Renamer functions.");
		return true;
	}

	int GetPerkRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*) {

		return 1;
	}

	bool ReevaluatePerk(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BSString a_ini) {

		return true;
	}
}