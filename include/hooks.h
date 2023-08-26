#pragma once 

namespace Hooks {

	//Other viable hook offsets: 0xB40, 0xC84, 0xD7C
	//Credit to NoahBoddie for the offsets:
	//      Github: https://github.com/NoahBoddie
	//   Nexusmods: https://www.nexusmods.com/skyrimspecialedition/users/13069030
	inline static REL::Relocation<std::uintptr_t> Get_Description_Hook1{ REL::ID(52527), 0xB40 };
	inline static REL::Relocation<std::uintptr_t> Get_Description_Hook2{ REL::ID(52527), 0xC84 };
	inline static REL::Relocation<std::uintptr_t> Get_Description_Hook3{ REL::ID(52527), 0xD7C };

	//Main manager class for the framework. Stores information, and triggers the hook.
	class DescriptionManager {

	public:
		static bool                         AddManagedPerkINI(std::string a_originalPerk, std::string a_transplantPerk, std::string a_newName, std::string a_configName);
		static bool                         AddManagedPerkPapyrus(RE::BGSPerk* a_originalPerk, RE::BGSPerk* a_transplantPerk, std::string a_newName);
		static DescriptionManager*          GetSingleton();
		static bool                         RemoveManagedPerkPapyrus(RE::BGSPerk* a_originalPerk); 


		//These two intercept the hook, and potentially change the result. Definitions found in PCH.h
		static void                                    thunk(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType);
		static inline REL::Relocation<decltype(thunk)> func;
		bool                                           InstallHook();

	private:
		inline static REL::Relocation<decltype(thunk)> DescriptionHook;
		inline static std::vector<RE::BGSPerk*>        ManagedPerks;

		//managedPerkData is a struct defined in PCH.h. Helps keep stuff readable.
		inline static std::vector<managedPerkData>     SwapPerkData;
	};
}