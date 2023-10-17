# Nippon Modding Framework
Nippon is a collection of tools designed to interact with the okami PC port. This project is in it's very early days and by all means not production ready.

<img src="Assets/Editor.png" width="100%"/>

## Starting the Editor
The editor requires 2 command line arguments. The first one is the directory where the okami data files are and the other is the BlowFish-256 key. Both arguments must be supplied as strings!
```ps1
Editor.exe "...\Steam\steamapps\common\Okami\data_pc" "YaKiNiKuM2rrVrPJpGMkfe3EK4RbpbHw"
```

## Compile The Project
First clone the repository via:
```
git clone --branch main --depth 1 https://github.com/allogic/Nippon
```
Launch the Visual Studio Solution, set the `Editor` as the startup project and build for `Debug` or `Release` bitness `x64`.
To run or debug the editor one must additionally adjust a few editor properties. Go into `Editor/Properties/Debugging` and set the following parameters for all configurations and platforms:
```
Command Arguments = "...\Steam\steamapps\common\Okami\data_pc" "YaKiNiKuM2rrVrPJpGMkfe3EK4RbpbHw"
Working Directory = $(SolutionDir)$(IntDir)
```

## Troubleshooting
 - If you got build errors, make sure you are using the latest Visual Studio version. (Tested with VS 2022 >= 17.X.X)
 - If for some reason the editor does not start, make sure the latest graphics card drivers are installed on your system.
 - If only black thumbnails appear, then one must first generate them via `MainMenu/Tools/Generate Thumbnails`. They should be visible after editor relaunch.

## Exporting Assets
Right click on a random actor that you with to export and choose `Export as Wavefront` to generate a wavefront object and material file. It should be ready to be imported into blender or other 3D software. Be sure to enable backface culling in your external rendering software to view the objects properly, otherwise only the black hull will be visible for most objects.

<img src="Assets/Oki.jpg" width="33%"/><img src="Assets/Crow.jpg" width="33%"/><img src="Assets/Rao.jpg" width="33%"/>

## Editor Scene Controls
- `WASD` to move
- `L-SHIFT` for lightspeed

## Issues / Pull Requests
If bugs are being found, please don't hesitate to open an issue or pull request!

## Resources
#### Links
- Wiki https://okami.speedruns.wiki
- Discord https://discord.gg/fbJdaeb
#### Repositories
- https://github.com/Shintensu/OkamiHD-Reverse-Engineering
- https://github.com/whataboutclyde/okami-utils
