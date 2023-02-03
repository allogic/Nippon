![](README.jpg)

# Nippon Modding Framework

Nippon is a cross platform game editor, currently designed to interact with the okami PC port. This project is in it's very early days and by all means not production ready.

## Setting Up Nippon

The editor requires a configuration file. Inside the `Binary` directory update the following paths inside `Config.json`.

```jsonc
{
  "gameDir": "/../Steam/steamapps/common/Okami",
  "unpackDir": "/../Nippon/Unpack",
  "repackDir": "/../Nippon/Repack",
}
```

## Compile The Project

All binaries built, will be put inside the `Binary` directory.

### Visual Studio

Launch Visual Studio and open the `Project` directory as a CMake project.

```
Project/Delete Cache and Reconfigure
Build/Build All
```

Select startup item ...

### CMake

From the `Project` directory, enter the following commands to build the entire project.

```sh
mkdir Build
cd Build
cmake -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ..
# cmake -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ ..
cmake --build .
# cmake --fresh .
```

## How To Run

Once all binaries are built, first launch the editor and start the integrity check under `Packer/Check Integrity`. After that unpack your game files with `Packer/Unpack`. Once all files are decrypted, one can jump into levels `Map/<Region>/<Level>`.

## troubleshooting

 - If you got build errors, make sure you are using the latest Visual Studio version. (Tested with VS 2022 >= 17.X.X)
 - If for some reason the editor does not start, make sure the latest graphics card drivers are installed on your system.

## Editor Controls

- `WASD` to move
- `L-SHIFT` for lightspeed

## Issues / Pull Requests

If bugs are being found, please don't hesitate to open an issue. Especially if you are using an exotic linux distribution.

## Resources

### Links

- Wiki https://okami.speedruns.wiki
- Discord https://discord.gg/fbJdaeb

### Repositories

- https://github.com/Shintensu/OkamiHD-Reverse-Engineering
- https://github.com/whataboutclyde/okami-utils
