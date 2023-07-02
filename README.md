<img src="README.jpg"/>

# Nippon Modding Framework

Nippon is a cross platform game editor, currently designed to interact with the okami PC port. This project is in it's very early days and by all means not production ready.

## Setting Up Nippon

The editor requires a configuration file. Inside the `Binary` directory update the following paths inside `Config.json`.

```jsonc
{
  "gameDir": "/../Steam/steamapps/common/Okami",
  "decryptDir": "/../Nippon/Decrypt",
  "encryptDir": "/../Nippon/Encrypt",
  "unpackDir": "/../Nippon/Unpack",
  "repackDir": "/../Nippon/Repack",
  "thumbnailDir": "/../Nippon/Thumbnail",
  "exportDir": "/../Nippon/Export",
}
```

## Compile The Project

#### Visual Studio

Launch Visual Studio and open the `Project` directory as a CMake project.

```
Project/Delete Cache and Reconfigure
Build/Build All
```

#### CMake

From the `Project` directory, enter the following commands to build the entire project.

```sh
mkdir Build
cd Build
cmake -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ..
# cmake -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ ..
cmake --build .
# cmake --fresh .
```

## Troubleshooting

 - If you got build errors, make sure you are using the latest Visual Studio version. (Tested with VS 2022 >= 17.X.X)
 - If for some reason the editor does not start, make sure the latest graphics card drivers are installed on your system.
 - If the editor crashes, be sure `Config.json` has POSIX compliant paths. (Even on windows)

## Decrypting / Unpacking Game Files

The game files need to be extracted first in order for the editor to work correctly. Using the main menu `Packer` option, all or only specific files can be decrypted and unpacked. There is also an option to repack and encrypt them again but this feature is still in development.

After the game files have been decrypted and unpacked, level and entity scenes can be opened successfully.

## Entity Browser

With the help of the entity browser, game assets can be previewed and saved to disk. If only black images appear, then no thumbnails have yet been generated. Navigate to the main menu `Tool` option and generate them. They should be visible after editor relaunch. The `Export` function can be used to generate a wavefront object and material file which should be ready to be imported into blender or other 3D software. Be sure to enable backface culling to view the objects properly, otherwies only the black hull will be visible.

<img src="OKI.jpg" width="300" style="display:inline"/>
<img src="CROW.jpg" width="300" style="display:inline"/>
<img src="RAO.jpg" width="300" style="display:inline"/>

## Editor Scene Controls

- `WASD` to move
- `L-SHIFT` for lightspeed

## Issues / Pull Requests

If bugs are being found, please don't hesitate to open an issue. Especially if you are using an exotic linux distribution.
The linux build is currently broken since DirectXTex is only available on windows but a cross-platform implementation is on it's way.

## Resources

#### Links

- Wiki https://okami.speedruns.wiki
- Discord https://discord.gg/fbJdaeb

#### Repositories

- https://github.com/Shintensu/OkamiHD-Reverse-Engineering
- https://github.com/whataboutclyde/okami-utils
