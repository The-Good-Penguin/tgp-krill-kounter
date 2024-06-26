KrillKounter
-------------

# Description
A utility for monitoring and logging block device stats to a JSON file at a regular time interval.

# Dependencies
- make-4.3
- cmake-3.22.1
- libjson-glib-dev-1.6.6
- lsblk-2.37.2

# Compilation
1. Clone this repo to a local directory using the following command, `git clone https://github.com/The-Good-Penguin/tgp-krill-kounter.git` 
2. From inside the local repo directory, run the following command to create a build directory. `cmake -S . -B build`
3. From inside the local repo directory, run the following command to build the `KrillKounter` executable within the `build/` subdirectory. `cmake --build build`

# Installation
1. From inside the previously created `build/` subdirectory, run the command `sudo cmake --build . --target install`. This will install the executable to `/usr/bin/KrillKounter`, and the systemd service file to `/lib/systemd/system/KrillKounter.service`.
2. KrillKounter can be configured by editing `Enviroment` values within `/lib/systemd/system/KrillKounter.service`;

```
KK_CONFIG_PATH - Path to the JSON file to be used for configuring the daemon in JSON instead of configuring via the variables below

KK_STATS_PATH - Path to the JSON file to be used for logging of block device stats and info.

KK_DEVICE_PATH - Path to the target block device to be monitored by KrillKounter [e.g. /dev/sda]

KK_DEVICE_NAME - Name of the target block device to be monitored by KrillKounter [e.g. sda]

KK_UPDATE_RATE - Time interval to wait between JSON file updates, in seconds.
```

3. Enable and start the KrillKounter systemd service using the following command `systemctl enable --now KrillKounter.service`. The state of the KrillKounter service can be monitored using the command `systemctl status KrillKounter`.

## Config File Format
```JSON
{
    "devices": [
        "/dev/sda",
        "/dev/mmcblk0"
    ],
    "updateRate": 3600,
    "statsFilePath": "/usr/share/KrillKounter/stats.json"
}
```
devices is an array of device paths you wish to monitor

# Contributing
Issue a PR and follow the guidelines outlined in the CodingStyle.md