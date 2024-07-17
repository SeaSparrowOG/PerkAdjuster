#include <hooks.h>

namespace Hooks {
	bool Get_Description_Hook::Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(52527), 0xD7C };
		stl::write_thunk_call<Get_Description_Hook>(target.address());
		return true;
	}
	void Get_Description_Hook::thunk(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType)
	{
		func(a_this, a_out, a_parent, a_fieldType);
		//changing a_out here actually updates in the menu, this is great.
	}
}