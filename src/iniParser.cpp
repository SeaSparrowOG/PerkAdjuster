#include <iniParser.h>
#include <hooks.h>

namespace INIParser {

    bool loadINIs() {

		auto manager = Hooks::DescriptionManager::GetSingleton();
        auto directory = R"(Data\)";
        auto suffix = "_PRKR"sv;
        std::vector<std::string> paths = clib_util::distribution::get_configs(directory, suffix);

        if (paths.empty()) {

            SKSE::log::info("No INI files detected.");
            return false;
        }

		for (const auto& foundPath : paths) {

			CSimpleIniA ini;
			ini.SetUnicode();
			ini.SetMultiKey();

			if (const auto rc = ini.LoadFile(foundPath.c_str()); rc < 0) {

				SKSE::log::info("Failed to open INI: {}.", foundPath);
				continue;
			}

			std::list<CSimpleIniA::Entry> entries;
			ini.GetAllSections(entries);

			auto dataHandler = RE::TESDataHandler::GetSingleton();
			auto perk_array = dataHandler->GetFormArray<RE::BGSPerk>();

			SKSE::log::info("Processing: {}", foundPath);
			for (auto& localEntry : entries) {

				auto entryName = localEntry.pItem;
				auto entryDescription = ini.GetValue(entryName, "description");
				auto entryNewName = ini.GetValue(entryName, "name");

				if (manager->AddManagedPerkINI(entryName, entryDescription, entryNewName, foundPath)) {

					SKSE::log::info("New swap: {} to be swapped with < {} >.", entryName, entryDescription);
				}
			}
		}
	
        return true;
    }
}