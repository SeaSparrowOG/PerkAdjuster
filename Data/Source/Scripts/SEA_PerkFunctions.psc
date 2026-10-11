Scriptname SEA_PerkFunctions Hidden
{Script for Perk Adjuster's API. To use, <import SEA_PerkFunctions> in your own script.}

Int[] Function GetVersion() Global Native
Bool Function AddPerkToTree(Perk a_kPerk, ActorValueInfo a_kAV, Float a_x, Float a_y, Perk[] a_parents = NONE, Perk[] a_children = NONE) Global Native
Bool Function RemoveFromPerkTree(Perk a_kPerk, ActorValueInfo a_kAV) Global Native