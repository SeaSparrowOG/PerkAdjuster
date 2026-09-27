#include "papyrus.h"

#include "perkManipulator.h"

namespace Papyrus {
	static std::vector<int> GetVersion(STATIC_ARGS) {
		return { Plugin::VERSION[0], Plugin::VERSION[1], Plugin::VERSION[2] };
	}

	bool AddPerkToTree(STATIC_ARGS, RE::BGSPerk* a_perk,
		RE::ActorValueInfo* a_info,
		float a_x, float a_y,
		std::vector<RE::BGSPerk*> a_parents,
		std::vector<RE::BGSPerk*> a_children) {
		
		if (!a_perk || !a_info) {
			return false;
		}

		return PerkManipulation::Manipulator::AddPapyrusPerk(a_perk,
			a_info, a_x, a_y, a_parents, a_children);
	}

	bool RemoveFromPerkTree(STATIC_ARGS, RE::BGSPerk* a_perk, RE::ActorValueInfo* a_info) {
		if (!a_perk || !a_info) return false;
		return PerkManipulation::Manipulator::RemovePapyrusPerk(a_perk, a_info);
	}

	bool Bind(VM& a_vm)
	{
		BIND(GetVersion);
		BIND(AddPerkToTree);
		BIND(RemoveFromPerkTree);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}
