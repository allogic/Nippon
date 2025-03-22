# Nippon Modding Framework
Nippon is a collection of tools designed to interact with the okami PC port. This project is in it's very early days and by all means not production ready.

<img src="Assets/Editor.png" width="100%"/>

## Starting The Editor
On first start the editor will ask for the okami data directory.

It should look something along like `C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc`.

#### Editor Scene Controls
- `WASD/QE` to move
- `L-SHIFT` for lightspeed
- `F` reset position

## Compile The Project
First clone the repository and execute the setup which should create all missing folders and files.
```
git clone --branch main --depth 1 https://github.com/allogic/Nippon
Setup.ps1
```
Launch the Visual Studio Solution, set the `Editor` as the startup project and build for `Debug` or `Release` bitness `x64`.
If you want to launch the editor through Visual Studio change the Debugging Working Directory in the Editor Project to $(TargetPath)

#### Troubleshooting
 - If you got build errors, make sure you have the latest Visual Studio platform toolset v143 installed on your system.
 - If for some reason the editor does not start, make sure the latest graphics card drivers are installed on your system.

## How do I modify archives?
There is a standalone archive utility which can be utilized to quickly inspect or change archives. Make sure that you do not inadvertently overwrite existing game files.
```
Usage:

 ArchiveUtility [Command] [Arguments]

Commands:
 PrintToC         [Archive(Str)]                                     Print the table of content for AKT/BIN/CMP/DAT/DDP/EFF/IDD/PAC/SCP/TBL files
 PrintOfType      [Archive(Str)] [Type(Str)]                         Print all entries of type
 Extract          [Archive(Str)] [Folder(Str)]                       Extract an archive to disk
 Unfold           [Archive(Str)] [Folder(Str)]                       Unfold an archive to disk
 GetByTypeAndName [Archive(Str)] [Type(Str)] [Name(Str)] [File(Str)] Get an archive entry identified by type and name into a file
 SetByTypeAndName [Archive(Str)] [Type(Str)] [Name(Str)] [File(Str)] Set an archive entry identified by type and name from a file
 Version                                                             Print the current version
 Help                                                                Print this help message

Examples:
 PrintToC "r301.dat"
 PrintOfType "r301.dat" "DDS"
 Extract "r301.dat" "r301"
 Unfold "r301.dat" "r301"
 GetByTypeAndName "r301.dat" "DDS" "hyouzan" "hyouzan.DDS"
 SetByTypeAndName "r301.dat" "DDS" "hyouzan" "hyouzan.DDS"
```

## How do I modify models?
There is a standalone model utility which can be utilized to quickly inspect or change models. Have a look at the `Patches` folder to see a view examples.
```
Usage:

 ModelUtility [Command] [Arguments]

Commands:
 PrintToC                         [Model(Str)]                          Print the table of content for SCR/MD files
 PrintContent                     [Model(Str)]                          Print the content of SCR/MD files
 GenerateConversionRules          [Model(Str)] [Rules(Str)]             Generate conversion rules from an existing SCR/MD model
 ValidateFileAgainstRules         [File(Str)] [Rules(Str)]              Validate a standardized 3D file against conversion rules
 ConvertFileIntoProprietaryFormat [File(Str)] [Rules(Str)] [Model(Str)] Convert a standardized 3D file into the internal proprietary model format
 Version                                                                Print the current version
 Help                                                                   Print this help message

Examples:
 PrintToC "minka.SCR"
 PrintContent "minka.SCR"
 GenerateConversionRules "minka.SCR" "rules.json"
 ValidateFileAgainstRules "monkey.fbx" "rules.json"
 ConvertFileIntoProprietaryFormat "monkey.fbx" "rules.json" "monkey.SCR"
```
To change an existing model, you must first examine its properties. To do this, we first print out the ToC of an SCR/MD file. Next, we need to generate conversion rules that has the same values as we just saw in the ToC. Optionally we can validate our 3D file against the generated rules.

Make sure that your custom model has the same parent-child relationships. Also make sure that your custom model has the same vertex attributes as the source model.
 - VertexOffset <=> Vertex Position Strip
 - TextureMapOffset <=> UV's
 - TextureUvOffset <=> UV's
 - ColorWeightOffset <=> Vertex Colors

Finally, we can start the conversion process!

Here is a full example from the `Patches` folder.
```
Copy-Item -Path "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00_orig.dat" -Destination ".\ut00.dat"

.\ArchiveUtility.exe GetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\ut00.MD"
.\ModelUtility.exe GenerateConversionRules ".\ut00.MD" ".\Rules.json"
.\ModelUtility.exe ValidateFileAgainstRules ".\Tree.fbx" ".\Rules.json"
.\ModelUtility.exe ConvertFileIntoProprietaryFormat ".\Tree.fbx" ".\Rules.json" "Tree.MD"
.\ArchiveUtility.exe SetByTypeAndName ".\ut00.dat" "MD" "ut00" ".\Tree.MD"

Copy-Item -Path ".\ut00.dat" -Destination "C:\Program Files (x86)\Steam\steamapps\common\Okami\data_pc\ut\ut00.dat"
```

## Exporting Assets
Right click on a random entity that you wish to export and choose `Export as Wavefront` to generate a wavefront object and material file along with all referenced textures. It should be ready to be imported into blender or any other 3D modeling software. Be sure to enable backface culling in your external rendering software to view the objects properly, otherwise only the black hull will be visible for most objects.

<img src="Assets/Oki.jpg" width="33%"/><img src="Assets/Crow.jpg" width="33%"/><img src="Assets/Rao.jpg" width="33%"/>

## Issues / Pull Requests
If bugs are being found, please don't hesitate to open an issue or pull request!

## Resources

#### Links
- Wiki https://okami.speedruns.wiki
- Discord https://discord.gg/fbJdaeb

#### Repositories
- https://github.com/Shintensu/OkamiHD-Reverse-Engineering
- https://github.com/whataboutclyde/okami-utils

#### Tools
- https://github.com/WerWolv/ImHex
- https://github.com/rizinorg/cutter

#### Old File Extension List
```
AFD - unknown purpose but are small and contain no multimedia.
AFS - if an AFS file is a subfile than it only contains audio, usually a few. Game Extractor can extract these but if you have try after you exact the big one (if some one can optimize the script then this should be displayed as a folder)
BIN - These are sub archives that Game Extractor can't really extract properly. The format is easy the first 4bytes are the number of files. then the next 4 bytes are all of the file offsets, then there are text extensions (4bytea for all of the files possible extensions include:
ANS - These are blank 16bytes strange
AKT - Contain Collision Maps and Terrain type Maps (ie is is dirt, water mud ect)
BMH - Contain mostly floats possibly level terrain related contain the header BMH
CAM - Unknown purpose contain the header mtb3 (in lowercase)
CCH - Contain mostly floats probably map related no real header
DRH - Related or identical to the SCH files, no real header mostly floats
EFF - are similar or identical to the BIN archives
FI2 - These contain object placement data
ITS - Contain no floats possibly map related no real header
LI3 - Contain some floats unknown purpose
MEH - Are small contain mostly floats have the header MEH
MOT - Unknown Contain the header mtb3 (in lowercase)
MSD - which is itself a sub-sub file archive. It contains no header and begins with the relative positions of the files it contains. Maybe it is collision map data, but it does not use floats
MSA - Contains no header other than the size of the whole file, no floats maybe texture meta data
MST - really small no header no file size no floats ect
RNI - These are small, Unknown purpose no real header
SCH - These contain mostly Floats probably related to map boundary lines-contain no real header
SCR - These contain the Header scr (in lowercase) these are model archives for level terrain that use 16bytesigned vertexes similar to MD archives that contain character meshes
SEQ - Probably animation data
T32 - These are some sort of Tim2 textures but DO NOT contain the text header TIM2
T3L - same as BIN archive
TM2 - These are regular Tim2 textures that DO contain the standard TIM2 text header
TRE - Unknown related to TSC contain no real header
TS  - Possibly related to camera movement these contain the header TS
TSC - Unknown releated to TSC contain no real header
TST - Same as the BIN archive
BMT - These are small files unknown purpose
D   - The ones on the demo are archives that only contain 1 of each of the following
MPD - contain Meta data for the textures no real header
DAT - similar to BIN archives contain many files and models:
MD  - Contain the Header scr (in lowercase) + five 00 bytes+ the number of models (4bytes)+ four 00 bytes + The offsets to the models
MDB - These are the models packages and contain the submeshes+yet another type of Tim2 textures
DIR - a small text file with the name of the path to the video files
S   - some type of audio or video adx format??? this file might be the promational video for the full version of okami
SES - Audio
SFD - Audio
TBL - (map.tbl) This looks like a source code map, or at least ALL of the functions in the ELF file (PS2 Excutable file) this could easily be into hacks, but the file might not be included in the full version of the game
TM2 - (moji8.tm2) a Tim2 texture of the letters used in for menus and character speech.
```
