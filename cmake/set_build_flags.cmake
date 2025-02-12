# This file defines compiler and linker flags. To use it simply include it in your root CMakeLists.txt file.

if (NOT CACHE_INITIALIZED)
  # Only set variables on first CMake run to avoid overwriting user modifications in CMakeCache.txt.
  set(CACHE_INITIALIZED "yes" CACHE INTERNAL "Are initial values set?")

  # Overwriting CMake default values.

  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # enable position independent code (-fPIC) on all targets
  set(BUILD_SHARED_LIBS OFF) # build all libraries as static libs unless explicitly specified as shared
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<IN_LIST:$<CONFIG>,$<FILTER:${DEBUG_CONFIGURATIONS},EXCLUDE,ReleaseAsserts>>:Debug>") # set static runtime linkage (/MT or /MTd) for MSVC

  # The following option defaults to treating warnings as errors, however in cases where warnings prevent the user
  # to successfully build a target it can set to "OFF" by adding "-D WARNINGS_AS_ERRORS:BOOL=OFF" to the CMake command line.
  option(WARNINGS_AS_ERRORS "This switch activates or deactivates that warnings are treated as errors." ON)
  set(WARNINGS_AS_ERRORS OFF)

  if (CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    # * enables many other floating-point optimizations (-ffast-math)
    # * allow re-association of operands in series of floating-point operations (-fassociative-math)
    # * allow the reciprocal of a value to be used instead of dividing by the value if this enables optimizations (-freciprocal-math)
    # * disable some clang optimizations causing SSE floating point exceptions in the AC-4 core encoder (-ftrapping-math)
    # * enable all the warnings about constructions that some users consider questionable, and that are easy to avoid (-Wall)
    # * enable some extra warning flags that are not enabled by -Wall (-Wextra)
    # * don't warn about string constants that are longer than the “minimum maximum” length specified in the C standard (-Wno-overlength-strings)
    # * detects uninitialized values, can produce false positives !!! (-Wconditional-uninitialized)
    set(SHARED_FLAGS "-ffast-math -fassociative-math -freciprocal-math -ftrapping-math -Wall -Wextra -Wno-overlength-strings -Wconditional-uninitialized")

    # * full optimization (-O3)
    # * determine the language standard to C99 (-std=gnu99)
    # * warn if a function is declared or defined without specifying the argument types (-Wstrict-prototypes)
    # * warn if a global function is defined without a previous prototype declaration (-Wmissing-prototypes)
    # * warn when a declaration is found after a statement in a block (-Wdeclaration-after-statement)
    # * means “no optimization”: this level compiles the fastest and generates the most debuggable code (-O0)
    # * produce debugging information in the operating system’s native format (-g)
    # * ignore deprecation warnings (-Wno-deprecated-declarations)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wno-deprecated-declarations ${SHARED_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wno-deprecated-declarations ${SHARED_FLAGS}")

    # * since gcc is used as frontend for ld, O2 might imply some optimizations during the linking step (-O2, taken from mbuild settings)
    # * dead code stripping - OSX clang actually use gnu linker, which supports (-dead_strip)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-O2 -dead_strip")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-dead_strip")


  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # * enables many other floating-point optimizations (-ffast-math)
    # * allow re-association of operands in series of floating-point operations (-fassociative-math)
    # * allow the reciprocal of a value to be used instead of dividing by the value if this enables optimizations (-freciprocal-math)
    # * compile code assuming that floating-point operations cannot generate user-visible traps (-fno-trapping-math)
    # * disable finite symbols that were removed in later glibc versions (-fno-finite-math-only)
    # * enable all the warnings about constructions that some users consider questionable, and that are easy to avoid (-Wall)
    # * enable some extra warning flags that are not enabled by -Wall (-Wextra)
    # * warn if a variable-length array is used in the code (-Wvla)
    set(SHARED_FLAGS "-ffast-math -fassociative-math -freciprocal-math -fno-trapping-math -fno-finite-math-only -Wall -Wextra -Wvla")

    # * full optimization (-O3)
    # * reduce compilation time and make debugging produce the expected results (-O0)
    # * produce debugging information in the operating system’s native format (-g)
    # * determine the language standard to C99 (-std=gnu99)
    # * warn if a function is declared or defined without specifying the argument types (-Wstrict-prototypes)
    # * warn if a global function is defined without a previous prototype declaration (-Wmissing-prototypes)
    # * warn when a declaration is found after a statement in a block (-Wdeclaration-after-statement)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${SHARED_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wdeclaration-after-statement ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g ${SHARED_FLAGS}")

    # * remove all symbol table and relocation information from the executable (-s)
    # * since gcc is used as frontend for ld, O2 might imply some optimizations during the linking step (-O2, taken from mbuild settings)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-O2 -s")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")


  elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # * allow the compiler to reorder, combine, or simplify floating-point operations to optimize floating-point code for speed and space (/fp:fast)
    # * enable warning level 4 (/W4)
    # * disable C-Runtime secure functions warnings (_CRT_SECURE_NO_WARNINGS)
    set(SHARED_FLAGS "/fp:fast /W4 /D \"_CRT_SECURE_NO_WARNINGS\"")

    # * maximize speed (/O2)
    # * no optimization (/Od)
    # * disable detection of stack-buffer over-runs in release (/GS-)
    # * produces a separate PDB file that contains all the symbolic debugging information for use with the debugger (/Zi)
    # * disable C++ standard library deprecation warnings (_SCL_SECURE_NO_WARNINGS)
    set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /GS- /D \"NDEBUG\" ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE  "/D \"_SCL_SECURE_NO_WARNINGS\" ${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_DEBUG "/MTd /Od /Zi ${SHARED_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D \"_SCL_SECURE_NO_WARNINGS\" ${CMAKE_C_FLAGS_DEBUG}")

    # * disable generation of table with safe exception handlers (/SAFESEH:NO)
    # * generate debug information (/DEBUG)
    # * disable incremental linking in debug (/INCREMENTAL:NO)
    # * ignore libcmt.lib in debug (/NODEFAULTLIB:libcmt.lib)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /SAFESEH:NO /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib")
  else ()
    message(FATAL_ERROR "\nERROR: Compiler ${CMAKE_C_COMPILER_ID} is not supported.\n")
  endif ()

  if (WARNINGS_AS_ERRORS)
    if (MSVC)
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /WX")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /WX")
      set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /WX")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /WX")
    else ()
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Werror")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
      set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    endif ()
    message("-- Warnings are treated as errors. Add '-D WARNINGS_AS_ERRORS:BOOL=OFF' to disable that behavior.")
  endif ()

endif()

# Save current build flags to CMakeCache.txt:

set(CMAKE_POSITION_INDEPENDENT_CODE       "${CMAKE_POSITION_INDEPENDENT_CODE}"       CACHE STRING "enable position independent code (-fPIC) on all targets" FORCE)
set(BUILD_SHARED_LIBS                     "${BUILD_SHARED_LIBS}"                     CACHE STRING "build all libraries as static libs unless explicitly specified as shared" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY            "${CMAKE_MSVC_RUNTIME_LIBRARY}"            CACHE STRING "MSVC runtime linkage" FORCE)
set(WARNINGS_AS_ERRORS                    "${WARNINGS_AS_ERRORS}"                    CACHE STRING "Treat warnings as errors" FORCE)

set(CMAKE_C_FLAGS_RELEASE                 "${CMAKE_C_FLAGS_RELEASE}"                 CACHE STRING "C compiler flags for RELEASE build" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE               "${CMAKE_CXX_FLAGS_RELEASE}"               CACHE STRING "C++ compiler flags for RELEASE build" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE        "${CMAKE_EXE_LINKER_FLAGS_RELEASE}"        CACHE STRING "Linker flags for RELEASE build" FORCE)


set(CMAKE_C_FLAGS_DEBUG                   "${CMAKE_C_FLAGS_DEBUG}"                   CACHE STRING "C compiler flags for DEBUG build" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG                 "${CMAKE_CXX_FLAGS_DEBUG}"                 CACHE STRING "C++ compiler flags for DEBUG build" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG          "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"          CACHE STRING "Linker flags for DEBUG build" FORCE)
