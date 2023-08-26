# Perk Renamer
## Introduction
Perk Renamer is an SKSE-based framework for dynamically editing the name and descriptions of perks. It can do so based on INI files at the start of the game, or mid game through the new Papyrus functions. 
## Building the project
In order to build the project, you will need a working installation of [Visual Studio](https://visualstudio.microsoft.com/), and [VCPKG](https://github.com/microsoft/vcpkg). After installing VCPKG, make sure that you create the VCPKG_ROOT Environment Variable. Optionally, set the SKYRIM_MODS_FOLDER variable, and make it point to your MO2 mods directory to automatically deploy the built DLL in an MO2 mod. Image for reference:

![A showcase of the environmet variables setup.](https://cdn.discordapp.com/attachments/625292279468523522/1144881752364564560/Variables.jpg)
After that, simply clone the repository, open it, and right click -> open with Visual Studio. Make sure that you choose a MSVC compiler (Release or Debug), otherwise this won't compile. For testing purposes, I recommend Debug (MSVC). Dependencies should be downloaded automatically from this point forward.
## Creating a swap INI
In order to start testing, you will want to set up a swap first. The easiest way to do this is to create a swap INI. These INI files are evaluated once the data loads, and have fairly verbose logging. If you have worked with any of PO3's frameworkd (KID/SPID/BOS), you will be pretty familiar with the concept. Simply create a INI file ending in "_PRKR", and drop it in the Data folder. If you are using a mod manager (you should), you can even include it as a mod. Here is an example of how that can look in MO2:
![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1144884246436126760/Screenshot_2.png)
The contents of the INI are split into different Sections. Each Section contains a name and a description key. The Section should correspond with the EditorID of the perk you are attempting to change. The name key will be the new name of the perk. The description key should hold the EditorID of the perk that will hold the new description. An example would be this:

    [Alchemist00]
    name=Herbalist
    description=QuickReflexes

This entry will change the first rank of Alchemist to be named "Herbalist" and will change its description to the description of Quick Reflexes. You can see an example here:
![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1144887067264950282/swapExample.jpg)
## Limitations
As with any pre-release, there are several limitations to keep in mind.

 1. The PapyrusAPI is untested.
 2. The INI swaps require you to use the original EditorID of the perks you are swapping. This means that even though mods like Adamant may change the EditorID of the perks, Perk Renamer will still refer to the original perk EditorIDs (this will be addressed soon). What this effectively means is that the example swap will work like this when Adamant is installed:

![enter image description here](https://cdn.discordapp.com/attachments/625292279468523522/1144888650740543510/adamantSwapExample.jpg)
## Feedback
IDK, open an issue, I guess? Alternatively, you can try finding me on Discord (.seasparrow).
