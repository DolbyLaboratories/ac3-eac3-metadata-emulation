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

There is a bash test script test.sh that creates DRC plots for both line mode and RF mode.
Once debug build is complete run using:
``` ./test.sh ```

Plots should be created for each artistic compression profile in line mode and RF mode so 10 plots should be created as pdfs in the test/output/ folder.

# Tools

To support testing executables are provided to encode and decode AC3 and EC3 files. A graphplotter utility and pwr_vs_time tool from SATS are provided. The test script should run without the need for any external additional tools.