#pragma once 

namespace Hooks {

	//Credit to NoahBoddie for the offsets:
	//   Github: https://github.com/NoahBoddie
	//   Nexusmods: https://www.nexusmods.com/skyrimspecialedition/users/13069030
	struct Get_Description_Hook {
		static bool Install();
		static void thunk(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType);

		inline static REL::Relocation<decltype(thunk)> func;
	};
}