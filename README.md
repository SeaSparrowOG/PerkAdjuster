# Perk Adjuster
## Purpose 
Perk Adjuster is an SKSE framework for the manipulation of perks. It allows for easy name changing while maintaining a high compatibility profile and perk placement.
## Usage
Perk Adjuster works on a set of rules. Rules are defined in `Data\SKSE\Plugins\PerkAdjuster\` as json files.
```json
{
    "descriptions": [
      {
        "plugins": [
          "Plugin1.esm"
        ],
        "changes": [
          {
            "perk": "0xFormID|PluginSource.esp",
            "newDescription": "..."
          }
        ]
      }
    ],
    "additions": [
      {
        "perk": "0xFormID|PluginSource.esp",
        "skill": "0xFormID|PluginSource.esp",
        "x": 1.0,
        "y": 0.5,
        "parents": [
          "0xFormID|PluginSource.esp"
        ],
        "children": [
          "0xFormID|PluginSource.esp"
        ]
      }
    ]
}
```
1. Descriptions
Descriptions defines description changes. Given a perk in the form of a formatted string (`0xFormID|PluginSource.esp`) or an EditorID (`PerkEDID`) if PowerOfThree's Tweaks are installed, the perk's description is changed to the specified one.
1. Additions
Additions defines new perks. Given a perk, skill, position, and optionally parents and children the framework will place the given perk in the tree and connect it to its parents and children.
## Building
### Requirements 
1. Install VCPKG and add its installation location in an environment variable called "VCPKG_ROOT".
2. Install CMake.
3. Install VS with C++ Desktop Development
4. Clone this repository.
5. Open a Git Bash/Powershell Window in the project folder, and run this command: `git submodule update --init --recursive`
6. Open the project in VS, wait for CMake to finish, and build the project.
### Automagically deploy
If you want to deploy directly in a MO2 instance, you can add the mods folder of that instance in an environment variable called "SKYRIM_MODS_FOLDER". Upon building, it will automatically create a mod folder in that location with the built DLL inside.