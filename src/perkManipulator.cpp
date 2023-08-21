#include <perkManipulator.h>

namespace manipulate {

	bool AdjustPerk(RE::BGSPerk* a_perk, const char* a_newName, RE::BGSPerk* a_transplantPerk) {

		SKSE::log::info("Processing {}.", a_perk->fullName);

		a_perk->fullName = a_newName;
		a_perk->descriptionText = a_transplantPerk->descriptionText;
		return true;
	}
}