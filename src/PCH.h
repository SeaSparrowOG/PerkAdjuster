#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

//Needed for clib-util functions. Included via VCPKG, configured via CMake.
#include <windows.h>
#include <CLIBUtil/utils.hpp>

using namespace std::literals;

//Used to bind Papyrus Functions.
#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, script, a_method __VA_OPT__(, ) __VA_ARGS__)
using VM = RE::BSScript::Internal::VirtualMachine;
using StackID = RE::VMStackID;
inline constexpr auto script = "SEA_PerkRenamer"sv;

//Used for more code readability - PO3 Style.
template <class T>
void write_thunk_call(std::uintptr_t a_src)
{
	auto& trampoline = SKSE::GetTrampoline();
	SKSE::AllocTrampoline(14);

	T::func = trampoline.write_call<5>(a_src, T::thunk);
}

//More code readability, my style.
struct managedPerkData {

	std::string  originalName;
	std::string  newName;
	std::string  newNamePapyrus;
	std::string  EDID;
	std::string  description;
	std::string  INIHandler;

	RE::BGSPerk* managedPerk;
	RE::BGSPerk* descriptionDonor;
	RE::BGSPerk* runtimeDescriptionDonor;

	
	managedPerkData() = default;
	void to_String() {

		SKSE::log::info("Managed Perk Data:.");
		SKSE::log::info("Original Name:               {}.", originalName);
		SKSE::log::info("New Name:                    {}.", newName);
		SKSE::log::info("New Name (Papyrus):          {}.", newNamePapyrus);
		SKSE::log::info("EditorID:                    {}.", EDID);
		SKSE::log::info("Description:                 {}", description);
		SKSE::log::info("INI Handler:                 {}.", INIHandler);
		SKSE::log::info("Managed Perk:                {}.", managedPerk->GetName());
	}
};