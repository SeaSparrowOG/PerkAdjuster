#include "serde.h"
#include "perkManipulator.h"

namespace Serialization {
	void Revert() {
		auto& papyrusPerks = PerkManipulation::Manipulator::storedPapyrusPerks;
		auto it = papyrusPerks.begin();
		auto end = papyrusPerks.end();

		for (; it != end; ++it) {
			auto* basePerk = (*it).basePerk;
			auto* target = (*it).targetValue;
			PerkManipulation::Manipulator::RemovePapyrusPerk(basePerk, target);
		}

		//probably not needed.
		papyrusPerks.clear();
	}

	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		if (!a_intfc->OpenRecord(StoredPerks, Version)) {
			_loggerError("Failed to open record for StoredCurrency");
			return;
		}

		/*
		Structure:
		1. Size
		2. Array of entries.

		Entry: [Perk formID, TargetAV formID, x, y, Size, vec<perk> parent formIDs, Size, vec<perk>]
		*/
		auto& storedPerks = PerkManipulation::Manipulator::storedPapyrusPerks;

		if (!a_intfc->WriteRecordData(storedPerks.size())) {
			_loggerError("Failed to write size of stored perks.");
			return;
		}

		for (const auto& [perk, target, x, y, parents, children] : storedPerks) {
			if (!a_intfc->WriteRecordData(perk->formID)) {
				_loggerError("Failed to write the formID of perk {}.", perk ? _debugEDID(perk) : "NULL");
				return;
			}

			if (!a_intfc->WriteRecordData(target->formID)) {
				_loggerError("Failed to write the formID of the ActorValueInfo {}.", target ? _debugEDID(target) : "NULL");
				return;
			}

			if (!a_intfc->WriteRecordData(x)) {
				_loggerError("Failed to write the x offset of perk {}.", perk ? _debugEDID(perk) : "NULL");
				return;
			}

			if (!a_intfc->WriteRecordData(y)) {
				_loggerError("Failed to write the y offset of perk {}.", perk ? _debugEDID(perk) : "NULL");
				return;
			}

			if (!a_intfc->WriteRecordData(parents.size())) {
				_loggerError("Failed to write the parent size of perk {}.", perk ? _debugEDID(perk) : "NULL");
				return;
			}

			for (auto& parent : parents) {
				if (!a_intfc->WriteRecordData(parent->formID)) {
					_loggerError("Failed to write the parent perk ID of perk {}.", perk ? _debugEDID(perk) : "NULL");
					return;
				}
			}

			if (!a_intfc->WriteRecordData(children.size())) {
				_loggerError("Failed to write the child size of perk {}.", perk ? _debugEDID(perk) : "NULL");
				return;
			}

			for (auto& child : children) {
				if (!a_intfc->WriteRecordData(child->formID)) {
					_loggerError("Failed to write the child perk ID of perk {}.", perk ? _debugEDID(perk) : "NULL");
					return;
				}
			}
		}
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		Revert();
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != Version) {
				_loggerError("Loaded data is incompatible with plugin version!");
			}

			if (type == StoredPerks) {
				/*
				Structure:
				1. Size
				2. Array of entries.

				Entry: [Perk formID, TargetAV formID, x, y, Size, vec<perk> parent formIDs, Size, vec<perk>]
				*/
				std::size_t storedDataLength;
				if (!a_intfc->ReadRecordData(storedDataLength)) {
					_loggerError("Failed to read size of stored data.");
					return;
				}

				for (std::size_t i = 0; i < storedDataLength; i++) {
					RE::BGSPerk* basePerk = nullptr;
					RE::ActorValueInfo* target = nullptr;
					float x = -1.0f;
					float y = -1.0f;
					std::vector<RE::BGSPerk*> parents;
					std::vector<RE::BGSPerk*> children;

					//Base perk start
					RE::FormID basePerkOldID;
					if (!a_intfc->ReadRecordData(basePerkOldID)) {
						_loggerError("Failed to read FormID");
						return;
					}

					RE::FormID basePerkID;
					if (!a_intfc->ResolveFormID(basePerkOldID, basePerkID)) {
						_loggerError("Failed to resolve FormID ({:08X})", basePerkOldID);
						return;
					}

					//AVInfo start
					RE::FormID targetOldID;
					if (!a_intfc->ReadRecordData(targetOldID)) {
						_loggerError("Failed to read ActorValueInfo formID.");
						return;
					}

					RE::FormID targetID;
					if (!a_intfc->ResolveFormID(targetOldID, targetID)) {
						_loggerError("Failed to resolve FormID ({:08X})", targetOldID);
						return;
					}

					//x/y start
					if (!a_intfc->ReadRecordData(x)) {
						_loggerError("Failed to get the proper x offset of perk {:08X}.", basePerkID);
						return;
					}

					if (!a_intfc->ReadRecordData(y)) {
						_loggerError("Failed to get the proper y offset of perk {:08X}.", basePerkID);
						return;
					}

					//parents start
					std::size_t vecLength;
					if (!a_intfc->ReadRecordData(vecLength)) {
						_loggerError("Failed to read size of stored parent data for perk {:08X}.", basePerkID);
						return;
					}

					for (std::size_t index = 0; index < vecLength; index++) {
						RE::BGSPerk* parent = nullptr;
						RE::FormID parentOldID;
						if (!a_intfc->ReadRecordData(parentOldID)) {
							_loggerError("Failed to read FormID for parent of perk {:08X}.", basePerkID);
							return;
						}

						RE::FormID parentID;
						if (!a_intfc->ResolveFormID(parentOldID, parentID)) {
							_loggerError("Failed to resolve FormID for parent of perk ({:08X})", basePerkID);
							return;
						}

						parent = RE::TESForm::LookupByID<RE::BGSPerk>(parentID);
						if (!parent) {
							_loggerError("Failed to find parent perk with ID {:08X} for perk {:08X}.", parentID, basePerkID);
							return;
						}

						parents.push_back(parent);
					}

					//children start
					if (!a_intfc->ReadRecordData(vecLength)) {
						_loggerError("Failed to read size of stored child data for perk {:08X}.", basePerkID);
						return;
					}

					for (std::size_t index = 0; index < vecLength; index++) {
						RE::BGSPerk* child = nullptr;
						RE::FormID childOldID;
						if (!a_intfc->ReadRecordData(childOldID)) {
							_loggerError("Failed to read FormID for child of perk {:08X}.", basePerkID);
							return;
						}

						RE::FormID childID;
						if (!a_intfc->ResolveFormID(childOldID, childID)) {
							_loggerError("Failed to resolve FormID for child of perk ({:08X})", basePerkID);
							return;
						}

						child = RE::TESForm::LookupByID<RE::BGSPerk>(childID);
						if (!child) {
							_loggerError("Failed to find child perk with ID {:08X} for perk {:08X}.", childID, basePerkID);
							return;
						}

						children.push_back(child);
					}

					target = RE::TESForm::LookupByID<RE::ActorValueInfo>(targetID);
					basePerk = RE::TESForm::LookupByID<RE::BGSPerk>(basePerkID);

					if (!basePerk) {
						_loggerError("Failed to find a perk with FormID {:08X}.", basePerkID);
						return;
					}
		
					if (!target) {
						_loggerError("Failed to find ActorValueInfo with FormID {:08X}.", targetID);
						return;
					}

					PerkManipulation::Manipulator::AddPapyrusPerk(basePerk, target, x, y, parents, children);
				}
			}
		}
	}

	void RevertCallback(SKSE::SerializationInterface* a_intfc)
	{
		Revert();
	}

	std::string DecodeTypeCode(std::uint32_t a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(std::uint32_t);

		std::string sig;
		sig.resize(SIZE);
		char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}
		return sig;
	}
}