#pragma once

namespace papyrusAPI {

	bool Bind(VM* a_vm);

	std::vector<int32_t> GetRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*);

	bool PerkReassignment(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BSString a_newName, RE::BGSPerk* a_transplantPerk);
}