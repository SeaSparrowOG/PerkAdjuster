#pragma once

namespace papyrusAPI {

	bool Bind(VM* a_vm);

	int GetPerkRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*);

	bool ReevaluatePerk(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BSString a_ini);
}