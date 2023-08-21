#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

//Include clib util
#include <windows.h>
#include <CLIBUtil/utils.hpp>

using namespace std::literals;

//Used to bind Papyrus Functions.
#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, script, a_method __VA_OPT__(, ) __VA_ARGS__
using VM = RE::BSScript::Internal::VirtualMachine;
using StackID = RE::VMStackID;
inline constexpr auto script = "SEA_PerkRenamer"sv;
