# Nippon Modding Framework

Nippon is a collection of cross platform tools, currently designed to interact with the okami PC port. This project is in pre pre alpha and by all means not production ready. 

## Setting Up Nippon

All tools require one configuration file. Inside the `Binary` directory update the following paths inside `Config.json`.

```jsonc
{
  "sourceDir": "/../Steam/steamapps/common/Okami", // Required only if you don't have unpacked files
  "unpackDir": "/../Nippon/Unpack",
  "repackDir": "/../Nippon/Repack",
}
```

## Compile The Project

From the `Project` directory, enter the following commands to build the entire project.

```sh
mkdir Build
cd Build && cmake ..
```

All binaries built, will be put inside the `Binary` directory.

## How To Run

Once all binaries are built, first launch the unpack tool to recursively decrypt all level files. After that, one can start the editor.

## Editor Controls

- `WASD` to move
- `L-SHIFT` for lightspeed

## Issues / Pull Requests

If bugs are being found, please don't hesitate to open an issue. Especially if you are using an exotic linux distribution.

## Resources

- Wiki https://okami.speedruns.wiki
- Discord https://discord.gg/fbJdaeb
