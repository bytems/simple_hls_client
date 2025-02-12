# This is a simple module to limit number of allowed build types (Debug, Release, etc.) and to
# validate provided build types. To use it simply include it in your root CMakeLists.txt file.

# Also defines C++ standard used for all projects

# Check whether the correct architecture is chosen for the MSVC generator.
if(MSVC)
    if (NOT (CMAKE_GENERATOR MATCHES ".*Win64" OR CMAKE_GENERATOR_PLATFORM MATCHES "x64"))
        message(FATAL_ERROR "\nERROR: Architecture either missing or wrong. Only 'Win64' is supported.\n")
    endif()
endif()

# Check if we're working with multi-configuration generator (e.g. MS Visual Studio) or
# single-configuration generator (e.g. make or ninja)
get_property(is_multi_config GLOBAL
    PROPERTY
        GENERATOR_IS_MULTI_CONFIG
)

# Define allowed build types.
set(allowed_build_types Debug Release)

# Code coverage requires gcc and gcov
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND allowed_build_types Coverage)
endif()

# Define debug and development configurations
set_property(GLOBAL APPEND PROPERTY DEBUG_CONFIGURATIONS Debug) # used by target_link_libraries()
get_cmake_property(DEBUG_CONFIGURATIONS DEBUG_CONFIGURATIONS) # convert property to variable
set(DEVELOPMENT_CONFIGURATIONS Debug Release)

# Set a default ('Debug') build type if none was specified
set(default_build_type "Debug")

if(CMAKE_GENERATOR STREQUAL "Xcode")
    # The project has targets with source files which vary by configuration.
    # This is not supported by the "Xcode" generator.

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS ${allowed_build_types})

    if((NOT CMAKE_CONFIGURATION_TYPES) OR (CMAKE_CONFIGURATION_TYPES MATCHES ";"))
        # Set default build type for Xcode generator, if not exactly one is set
        message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
        set(CMAKE_CONFIGURATION_TYPES "${default_build_type}" CACHE STRING "" FORCE)
    elseif(NOT CMAKE_CONFIGURATION_TYPES IN_LIST allowed_build_types)
        # Validate build type provided by the user
        message(FATAL_ERROR "Invalid build type: '${CMAKE_CONFIGURATION_TYPES}'")
    endif()
elseif(is_multi_config)
    # Define allowed build types for multi-configuration generators
    set(CMAKE_CONFIGURATION_TYPES ${allowed_build_types} CACHE STRING "" FORCE)
else()
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${allowed_build_types})

    if(NOT CMAKE_BUILD_TYPE)
        # If not specified, set default build type for single-configuration generators
        message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
        set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowed_build_types)
        # Validate build type provided by the user
        message(FATAL_ERROR "Invalid build type: '${CMAKE_BUILD_TYPE}'")
    endif()
endif()

# Tell CMake which build configuration (a.k.a. flavour) should be used,
# if imported, target does not provide given build configuration (flavour)
# Empty string will re-enable default behaviour of mapping imported targets without any flavour
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUGPROD Debug "")
set(CMAKE_MAP_IMPORTED_CONFIG_RELEASEPROD Release "")


# Set C++ standard
# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message("-- C++ Standard is ${CMAKE_CXX_STANDARD}")