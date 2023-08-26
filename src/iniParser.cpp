#include <iniParser.h>
#include <hooks.h>

namespace INIParser {

	RE::FormID ParseFormID(const std::string& str)
	{
		RE::FormID unResolvedFormID;
		RE::FormID resolvedFormID;

		std::istringstream ss{ str };
		ss >> std::hex >> resolvedFormID;

		if (resolvedFormID) {

			return resolvedFormID;
		}

		return unResolvedFormID;
	}

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
				auto originalPerk = ini.GetValue(entryName, "iOriginalPerkID");
				auto originalPerkOrigin = ini.GetValue(entryName, "sOriginalPerkFile");
				auto transplantPerk = ini.GetValue(entryName, "iTransplantPerkID");
				auto transplantPerkOrigin = ini.GetValue(entryName, "sTransplantPerkFile");
				auto newName = ini.GetValue(entryName, "sNewName");

				if (!originalPerkOrigin) {

					SKSE::log::info("Failed to read entry < {} > in INI [ {} ] - sOriginalPerkFile not found.", entryName, foundPath);
					continue;
				}

				if (!transplantPerkOrigin) {

					SKSE::log::info("Failed to read entry < {} > in INI [ {} ] - sTransplantPerkFile not found.", entryName, foundPath);
					continue;
				}

				if (!originalPerk) {

					SKSE::log::info("Failed to read entry < {} > in INI [ {} ] - iOriginalPerkID not found.", entryName, foundPath);
					continue;
				}

				if (!transplantPerk) {

					SKSE::log::info("Failed to read entry < {} > in INI [ {} ] - iTransplantPerkID not found.", entryName, foundPath);
					continue;
				}

				std::string newNameString;

				if (!newName) {

					SKSE::log::info("New name not specified in entry [ {} ] in INI < {} >. Will be using default name.", entryName, foundPath);
					newNameString = std::string();
				}
				else {

					newNameString = std::string(newName);
				}

				RE::FormID originalPerkFormID;
				originalPerkFormID = ParseFormID(originalPerk);

				if (!originalPerkFormID) {

					SKSE::log::info("Could not resolve FormID < {} > in [ {} ] in INI < {} >.", originalPerk, entryName, foundPath);
					continue;
				}

				RE::FormID transplantPerkFormID;
				transplantPerkFormID = ParseFormID(transplantPerk);

				if (!transplantPerkFormID) {

					SKSE::log::info("Could not resolve FormID < {} > in [ {} ] in INI < {} >.", transplantPerk, entryName, foundPath);
					continue;
				}

				if (manager->AddManagedPerkINI(originalPerkFormID, transplantPerkFormID, newNameString, foundPath, std::string(originalPerkOrigin), std::string(transplantPerkOrigin))) {

					SKSE::log::info("Swap processed successfully.");
				}
			}
		}
	
        return true;
    }
}