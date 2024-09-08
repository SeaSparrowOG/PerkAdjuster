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


		static bool        AddPapyrusPerk(RE::BGSPerk* a_perk, 
										RE::ActorValueInfo* a_info, 
										float a_x, float a_y, 
										std::vector<RE::BGSPerk*> a_parents,
										std::vector<RE::BGSPerk*> a_children);
		static bool        RemovePapyrusPerk(RE::BGSPerk* a_perk, RE::ActorValueInfo* a_target);

		struct PapyrusPerks {
			RE::BGSPerk* basePerk;
			RE::ActorValueInfo* targetValue;
			float x;
			float y;
			std::vector<RE::BGSPerk*> parents;
			std::vector<RE::BGSPerk*> children;

			PapyrusPerks(RE::BGSPerk* a_basePerk,
				RE::ActorValueInfo* a_AV,
				float a_x, float a_y,
				std::vector<RE::BGSPerk*> a_parents,
				std::vector<RE::BGSPerk*> a_children) {
				this->x = a_x;
				this->y = a_y;
				this->basePerk = a_basePerk;
				this->targetValue = a_AV;
				this->parents = a_parents;
				this->children = a_children;
			}
		};

		static inline std::vector<PapyrusPerks> storedPapyrusPerks;
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