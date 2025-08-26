# AC3_EC3 Metadata Emulation

This project provides AC-3 (Dolby Digital) and EC-3 (Dolby Digital Plus) metadata emulation. This functionality is similar to what was implemented originaly in the DP570 and then subsequently on the Cat 1100.

The code used here was taken directly from the DPF node as used in the Cat 1100.

# Prerequesites

The only supported target currently is Mac Arm64.

Conan is used to provide the libsndfile library. The easiest way to get this is via Python

```pip3 install conan```

Similar for cmake

```pip3 install cmake```

Sox is required for testing. On Mac the easiest way to install is via Homebrew
```brew install sox``` or download from Sourceforge:
https://sourceforge.net/projects/sox/

# Building the Executable

## Debug build

```
conan install . --output-folder=build_debug --build=missing -s build_type=Debug
cd build_debug
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
## Release build

```
conan install . --output-folder=build_release --build=missing -s build_type=Release
cd build_release
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

# Running
An executable will be created in the build folder. To get usage information run
```MdEmu -h```

The switches are designed to operate in a similar way to legacy command line encoders and decoders for ease of testing.


# Testing

SATS is required to be installed. This is available at 

The provided bash script test_drc.sh compares the drc graphs of the built executable with reference DRC graphs. A simple
pass fail is output on stdout. The script will create a build if required.

Run using:
``` ./test_drc.sh ```


# Tools
This contains a simple graph comparison utility used by the test script.
