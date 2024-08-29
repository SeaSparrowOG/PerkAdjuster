#pragma once

namespace PerkManipulation {
	class Manipulator {
	public:
		static bool        Install();
		static bool        ChangeDescription(std::string a_new, std::string a_old, int a_match);
		static void        PlaceNewPerk(RE::BGSPerk* a_newPerk, RE::ActorValueInfo* a_targetValue,
										   float a_x, float a_y,
										   std::vector<RE::BGSPerk*> a_parents,
										   std::vector<RE::BGSPerk*> a_children);
		static void        CreateLinks();
	private:
		static inline void GetDescription(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType);
		static inline REL::Relocation<decltype(&GetDescription)> _getDescription;
		static inline std::unordered_map<std::string, std::pair<std::string, int>> descriptionMatches;

		//This should be a struct.
		//Map:
		//    Key -> New node in tree
		//    Value -> Pair
		//        Pair First -> Parent perks (not nodes)
		//        Pair Second -> Child perks (not nodes)
		static inline std::unordered_map<RE::BGSSkillPerkTreeNode*, std::pair<std::vector<RE::BGSPerk*>, std::vector<RE::BGSPerk*>>> newLinks;
	};
}