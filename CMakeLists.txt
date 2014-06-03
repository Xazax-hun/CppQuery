cmake_minimum_required (VERSION 2.8.12)
project (CppQuery)

# Compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Libraries

# Boost
find_package(Boost REQUIRED)

# GTests
find_package(GTest)
if (GTEST_FOUND)
  enable_testing()
endif ()

# Doxygen support
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(Doxyfile.in
                 ${PROJECT_BINARY_DIR}/Doxyfile )

  add_custom_target (docs
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                     SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
                     COMMENT "Generating HTML documentation"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/Doxyfile")
  set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/docs")
endif ()

# Qt libraries
find_package(Qt5Widgets 5.2 REQUIRED)
set (CMAKE_AUTOMOC ON)

# Configurations

#To force 32 bit: -DARCH="32" 
if (NOT ARCH)
  set (ARCH "NATIVE")
endif ()

#To build debug: -DDEBUG="YES"
if (NOT DEBUG)
  set (DEBUG "NO")
endif ()

#Temporary hack
set (DEBUG "YES")

# use llvm-config to get the Clang and LLVM include dirs
execute_process (
  COMMAND llvm-config --src-root
  OUTPUT_VARIABLE LLVM_SRC_ROOT
)
STRING(REGEX REPLACE "(\r?\n)+$" "" LLVM_SRC_ROOT "${LLVM_SRC_ROOT}")

# ... and the object dir
execute_process (
  COMMAND llvm-config --obj-root
  OUTPUT_VARIABLE LLVM_OBJ_ROOT
)
STRING(REGEX REPLACE "(\r?\n)+$" "" LLVM_OBJ_ROOT "${LLVM_OBJ_ROOT}")

set (LLVM_BUILD_DIR "${LLVM_OBJ_ROOT}") # = `llvm-config --obj-root`
set (LLVM_INCLUDE_DIR "${LLVM_SRC_ROOT}/include") # = `llvm-config --src-root`/include
set (CLANG_INCLUDE_DIR "${LLVM_SRC_ROOT}/tools/clang/include") # = `llvm-config --src-root`/tools/clang/include
set (CLANG_GENERATED_INCLUDE_DIR "${LLVM_BUILD_DIR}/tools/clang/include")

# use llvm-config to get all flags we need to co-operate with LLVM and Clang
execute_process (
  COMMAND llvm-config --cxxflags
  OUTPUT_VARIABLE LLVM_CXX_FLAGS
)
STRING(REGEX REPLACE "(\r?\n)+$" "" LLVM_CXX_FLAGS "${LLVM_CXX_FLAGS}")
STRING(REGEX REPLACE "-DNDEBUG" "" LLVM_CXX_FLAGS "${LLVM_CXX_FLAGS}")

set (CPPQUERY_CXX_FLAGS "${LLVM_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-strict-aliasing -Wno-unused-parameter -Wno-parentheses -Wno-unknown-warning-option -pedantic -std=c++11 -fPIC -fexceptions -fno-rtti -Wno-unused-local-typedefs")

execute_process (
  COMMAND llvm-config --ldflags
  OUTPUT_VARIABLE LLVM_LD_FLAGS
)
STRING(REGEX REPLACE "(\r?\n)+$" "" LLVM_LD_FLAGS "${LLVM_LD_FLAGS}")

set (CPPQUERY_LD_FLAGS "${LLVM_LD_FLAGS}")

set (CLANGLIBS clangTooling clangFrontend clangSerialization clangDriver clangRewriteCore clangParse clangSema
           clangEdit clangAnalysis clangAST clangASTMatchers clangLex clangBasic clangDynamicASTMatchers LLVMTransformUtils
           LLVMCppBackendInfo LLVMMCParser LLVMMC LLVMCore LLVMBitWriter LLVMBitReader LLVMOption LLVMSupport pthread dl)

if (ARCH STREQUAL "32")
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -m32")
  set (CPPQUERY_LD_FLAGS "${CPPQUERY_LD_FLAGS} -m32")
endif ()

if (DEBUG STREQUAL "YES")
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -g1 -O0")
  set (PPDEFINITIONS "${PPDEFINITIONS} -D_DEBUG")
else ()
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -O3 -s")
endif ()

include_directories("include/" ${GTEST_INCLUDE_DIRS} ${CLANG_INCLUDE_DIR} ${LLVM_SRC_ROOT})

add_definitions (${PPDEFINITIONS})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPQUERY_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPPQUERY_LD_FLAGS}")

# Configuration file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/settings/settings.ini
               ${PROJECT_BINARY_DIR}/settings.ini COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/settings/catalog.txt
               ${PROJECT_BINARY_DIR}/catalog.txt COPYONLY)
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/settings.ini;${PROJECT_BINARY_DIR}/catalog.txt")

# Main executable
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE GUI_SOURCES ${PROJECT_SOURCE_DIR}/gui/*.cpp)
add_executable(CppQuery ${GUI_SOURCES} ${SOURCES})
target_link_libraries(CppQuery ${CLANGLIBS} Qt5::Widgets boost_filesystem boost_system)

set_source_files_properties(gui/query_completer.cpp PROPERTIES COMPILE_FLAGS "-g0 -O0 -fno-exceptions")

# Unit Tests
if (GTEST_FOUND)
  file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cpp)
  add_executable(unittest ${TEST_SOURCES})
  target_link_libraries(unittest ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CLANGLIBS})
  add_test(unittest unittest)
endif ()