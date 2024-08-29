#pragma once

namespace Settings {
	template <typename T>
	T* GetFormFromString(std::string a_str) {
		T* response = nullptr;

		if (const auto splitID = clib_util::string::split(a_str, "|"); splitID.size() == 2) {
			if (!clib_util::string::is_only_hex(splitID[0])) return response;
			const auto  formID = clib_util::string::to_num<RE::FormID>(splitID[0], true);

			const auto& modName = splitID[1];
			if (!RE::TESDataHandler::GetSingleton()->LookupModByName(modName)) return response;

			T* baseForm = RE::TESDataHandler::GetSingleton()->LookupForm<T>(formID, modName);
			return baseForm ? baseForm : response;
		}
		auto* form = RE::TESForm::LookupByEditorID<T>(a_str);
		if (form) return form;

		return response;
	}

	bool Read();
}