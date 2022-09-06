### Build Folder

- This folder contains the build scripts and firmware for most hardware and configuration combinations
- `XANADU_HV_EV` contains the bin and .elf files for the XANADU_HV_EV hardware. The build script for the same is present as rebuild_HV.sh
- The bootloader for the Xanadu-BMS : `generic_bootloader.bin`

- When making updates to the firmware it is a good idea to make sure that the rebuild_all script runs without errors, because that means that the build process is not broken.
