cmake_minimum_required(VERSION 3.15)
project(ParserBase)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${ParserBase_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${ParserBase_SOURCE_DIR}/CMake 
  ${ParserBase_SOURCE_DIR}/CMake/Modules
)
include(${ParserBase_SOURCE_DIR}/CMake/Configure.cmake)
if (NOT Configure_SUCCEEDED)
   return()
endif()



if (ParserBase_ExternalTarget)
    set(TargetFolders ${ParserBase_TargetFolders})
    set(TargetName    ${ParserBase_TargetName})
    set(TargetGroup   ${ParserBase_TargetGroup})
else()
    add_subdirectory(CMake)
    add_subdirectory(Internal/Utils)

    set(TargetFolders TRUE)
    set(TargetName ParserBase)
    set(TargetGroup )

    set(ParserBase_INCLUDE  ${ParserBase_SOURCE_DIR})
    set(ParserBase_LIBRARY  ${TargetName})
endif()

add_subdirectory(ParserBase)

if (ParserBase_BUILD_TEST)
    if (NOT ParserBase_ExternalTarget)
        add_gtest_source(Extern/ThirdParty)
    endif()

    set(TargetGroup Units)
    add_subdirectory(Test)
endif()
