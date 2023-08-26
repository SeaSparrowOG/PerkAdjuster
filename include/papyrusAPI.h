#pragma once

namespace PapyrusAPI {

	bool Bind(VM* a_vm);

	std::vector<int32_t> GetRenamerVersion(VM*, StackID a_StackID, RE::StaticFunctionTag*);

	bool ManipulatePerk(VM*, StackID a_StackID, RE::StaticFunctionTag*, RE::BGSPerk* a_perk, RE::BGSPerk* a_transplantPerk, RE::BSString a_newName);
}