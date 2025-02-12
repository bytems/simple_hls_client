# Simple HLS Client

Mohamed Saleh's implementation of fetching an HLS playlist, parsing the document, and sorting it. 
A common use case in the Disney Streaming Services Video Platform Engineering group.

## Limitations
-  I have only tested this on macOS. Cannot guarantee support for other platforms.
- Sorting is done in ascending order only

## Assumptions
- Only supporting tag types present in the given HLS playlist file.
- Fetched HLS playlist to remain unchanged. New sorted HLS playlist file is created.

## Prerequisites
* **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)
* **C++ Compiler** - needs to support at least the **C++11** standard, i.e. *MSVC*, *GCC*, *Clang*
* **CURL** - found at [everything curl](https://ec.haxx.se/install/index.html)


## Building Instructions
Generate the build system using a **CMake** routine similar to the one below:
```bash
cmake -S <source_directory> -B <build_directory>
```
- -S: Specifies the path to the directory containing the top-level CMakeList.txt
- -B: Specifies the path to the directory where build files will be generated

To build the project, all you need to do is run a **CMake** routine similar to the one below:
```bash
cd <build_directory>
cmake --build .
```

Alternatively, many IDEs have built-in support for CMake.

## Running Instructions


```bash
hls_fetch_and_sort
```

No command line options provided
