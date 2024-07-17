#include "hooks.h"
#include "nameHolder.h"

namespace Hooks {
	bool Get_Description_Hook::Install()
	{
		if (!NameHolder::NameHolder::GetSingleton()->ReadSettings()) return false;
		REL::Relocation<std::uintptr_t> target{ REL::ID(52527), 0xD7C };
		stl::write_thunk_call<Get_Description_Hook>(target.address());
		return true;
	}
	void Get_Description_Hook::thunk(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType)
	{
		func(a_this, a_out, a_parent, a_fieldType);
		auto* hookPerk = a_parent ? a_parent->As<RE::BGSPerk>() : nullptr;
		auto* holderSingleton = NameHolder::NameHolder::GetSingleton();
		bool managed = holderSingleton && hookPerk ? holderSingleton->IsManagedPerk(hookPerk) : false;
		if (!managed) return;

		auto newDescription = holderSingleton->GetNewDescription(hookPerk);
		if (newDescription.empty()) return;

		a_out = newDescription;
	}
}