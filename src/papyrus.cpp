#include "papyrus.h"

#include "perkManipulator.h"

namespace Papyrus {
	std::vector<int> GetVersion(STATIC_ARGS) {
		std::vector<int> response = std::vector<int>();
		response.push_back(Version::MAJOR);
		response.push_back(Version::MINOR);
		response.push_back(Version::PATCH);

		return response;
	}

	bool AddPerkToTree(STATIC_ARGS, RE::BGSPerk* a_perk,
		RE::ActorValueInfo* a_info,
		float a_x, float a_y,
		std::vector<RE::BGSPerk*> a_parents,
		std::vector<RE::BGSPerk*> a_children) {
		
		if (!a_perk || !a_info ||
			std::abs(a_x) * 2.0 > a_info->perkTreeWidth) {
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
