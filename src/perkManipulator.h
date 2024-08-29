#pragma once

namespace PerkManipulation {
	class Manipulator {
	public:
		static bool        Install();
		static bool        ChangeDescription(std::string a_new, std::string a_old, int a_match);
		static void        PlaceNewPerk(RE::BGSPerk* a_newPerk, RE::ActorValueInfo* a_targetValue,
										   float a_x, float a_y,
										   std::vector<RE::BGSSkillPerkTreeNode*> a_parents,
										   std::vector<RE::BGSSkillPerkTreeNode*> a_children);
	private:
		static inline void GetDescription(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType);
		static inline REL::Relocation<decltype(&GetDescription)> _getDescription;
		static inline std::unordered_map<std::string, std::pair<std::string, int>> descriptionMatches;
	};
}