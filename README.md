# Perk Renamer
## Introduction
Perk Renamer is an SKSE-based framework for dynamically editing the names and descriptions of perks. It can do so based on INI files at the start of the game, or mid game through the new Papyrus functions. 
## Building the project
In order to build the project, you will need a working installation of [Visual Studio](https://visualstudio.microsoft.com/), and [VCPKG](https://github.com/microsoft/vcpkg). After installing VCPKG, make sure that you create the VCPKG_ROOT Environment Variable. Optionally, set the SKYRIM_MODS_FOLDER variable, and make it point to your MO2 mods directory to automatically deploy the built DLL in an MO2 mod. Image for reference:

![A showcase of the environmet variables setup.](https://cdn.discordapp.com/attachments/625292279468523522/1144881752364564560/Variables.jpg)
After that, simply clone the repository, open it, and right-click -> open with Visual Studio. Make sure that you choose an MSVC compiler (Release or Debug), otherwise, this won't compile. For testing purposes, I recommend Debug (MSVC). Dependencies should be downloaded automatically from this point forward.
## Creating a swap INI
In order to start testing, you will want to set up a swap first. The easiest way to do this is to create a swap INI. These INI files are evaluated once the data loads and have fairly verbose logging. If you have worked with any of PO3's frameworks (KID/SPID/BOS), you will be pretty familiar with the concept. Simply create a INI file ending in "_PRKR", and drop it in the Data folder. If you are using a mod manager (you should), you can even include it as a mod. Here is an example of how that can look in MO2:
![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1144884246436126760/Screenshot_2.png)
The contents of the INI are split into different Sections. Each section is defined by a "Swap ID". This exists only to be a visual aid for you as you edit the INI files. I recommend using a naming convention to help you remember what perks are being edited. For example, this could be named "MAG_QuickCuts20 - QuickReflexes". There are 4 mandatory and 1 optional entries. The mandatory entries are OriginalPerkID (carries the FormID of the perk you want to swap its description, without the load order ID), TransplantPerkID (same, but for the perk that carries the new description), OriginalPerkFile and TransplantPerkFile that both carry the name of the files the perks are defined in. You can also specify a name entry, which will change the name of the original perk.

    [MAG_Herbalist - QuickReflexes]
    OriginalPerkID=0xBE127
    TransplantPerkID=0xD8C33
    OriginalPerkFile=Skyrim.esm
    TransplantPerkFile=Skyrim.esm
    name=Cool Perk

This entry will change the first rank of Alchemist to be named "Cool Perk" and will change its description to the description of Quick Reflexes. You can see an example here:
![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1145129413244092466/example.jpg)

![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1144888650740543510/adamantSwapExample.jpg)
## Feedback
IDK, open an issue, I guess? Alternatively, you can try finding me on Discord (.seasparrow).
