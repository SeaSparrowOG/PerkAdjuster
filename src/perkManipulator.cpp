#include "perkManipulator.h"

#include <unordered_set>

namespace {
	RE::BGSSkillPerkTreeNode* FindPerkNode(RE::BGSPerk* a_perk, RE::BGSSkillPerkTreeNode* a_node) {
		for (auto* child : a_node->children) {
			if (child->perk == a_perk) {
				return child;
			}

			RE::BGSSkillPerkTreeNode* foundNode = nullptr;
			foundNode = FindPerkNode(a_perk, child);
			if (foundNode) return foundNode;
		}

		return nullptr;
	}

	int32_t GetMaxIndex(RE::BGSSkillPerkTreeNode* node, std::unordered_set< const RE::BGSSkillPerkTreeNode*>& visited) {
		if (!node) return 0;
		if (visited.find(node) != visited.end()) return 0;
		visited.insert(node);

		int32_t index = node->index;

		for (const auto& child : node->children) {
			auto newIndex = GetMaxIndex(child, visited);
			if (newIndex > index) {
				index = newIndex;
			}
		}

		return index;
	}
}
namespace PerkManipulation {
	bool Manipulator::Install()
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<std::uintptr_t> target{ REL::ID(52527), 0xD7C };
		_getDescription = trampoline.write_call<5>(target.address(), &GetDescription);
		return true;
	}

	bool Manipulator::ChangeDescription(std::string a_new, std::string a_old, int a_match)
	{
		if (a_new.empty() || a_old.empty()) {
			return false;
		}
		if (descriptionMatches.contains(a_new) && descriptionMatches.at(a_old).second < a_match) {
			return false;
		}

		descriptionMatches[a_old] = std::make_pair(a_new, a_match);
		return true;
	}

	void Manipulator::PlaceNewPerk(RE::BGSPerk* a_newPerk, 
		RE::ActorValueInfo* a_targetValue,
		float a_x, float a_y, 
		std::vector<RE::BGSPerk*> a_parents,
		std::vector<RE::BGSPerk*> a_children)
	{
		auto visitedNodes = std::unordered_set< const RE::BGSSkillPerkTreeNode*>();
		int32_t index = GetMaxIndex(a_targetValue->perkTree, visitedNodes) + 1;

		auto* newNode = new RE::BGSSkillPerkTreeNode(index, a_targetValue);
		newNode->perk = a_newPerk;
		newNode->perkGridX = std::floor(a_x);
		newNode->perkGridY = std::floor(a_y);
		newNode->horizontalPosition = a_x - newNode->perkGridX;
		newNode->verticalPosition = a_y - newNode->perkGridY;

		newLinks[newNode] = std::make_pair(a_parents, a_children);
		a_targetValue->perkTree->children.push_back(newNode);
		newNode->parents.push_back(a_targetValue->perkTree);
	}

	void Manipulator::CreateLinks()
	{
		for (auto& entry : newLinks) {
			auto* newPerk = entry.first;
			auto& parents = entry.second.first;
			auto& children = entry.second.second;

			auto* rootNode = newPerk->associatedSkill->perkTree;

			for (auto* parent : parents) {
				auto* correspondingNode = FindPerkNode(parent, rootNode);
				if (!correspondingNode) continue;

				newPerk->parents.push_back(correspondingNode);
				correspondingNode->children.push_back(newPerk);
			}

			for (auto* child : children) {
				auto* correspondingNode = FindPerkNode(child, rootNode);
				if (!correspondingNode) continue;

				newPerk->children.push_back(correspondingNode);
				correspondingNode->parents.push_back(correspondingNode);
			}
		}
	}

	void Manipulator::GetDescription(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType)
	{
		_getDescription(a_this, a_out, a_parent, a_fieldType);

		std::string inString = a_out.c_str();
		if (!descriptionMatches.contains(inString)) return;

		a_out = descriptionMatches[inString].first;
	}
}