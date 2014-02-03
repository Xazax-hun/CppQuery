cmake_minimum_required (VERSION 2.8.12)
project (CppQuery)

# Libraries

# GTests
find_package(GTest REQUIRED)
enable_testing()

# Doxygen support
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(Doxyfile.in
                 ${PROJECT_BINARY_DIR}/Doxyfile )

  add_custom_target (docs
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                     SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
                     COMMENT "Generating HTML documentation"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/Doxyfile")
  set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_BINARY_DIR}/docs")
endif()

# Qt libraries
find_package(Qt5Widgets 5.2 REQUIRED)
set(CMAKE_AUTOMOC ON)

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

set (CPPQUERY_CXX_FLAGS "${LLVM_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-strict-aliasing -Wno-unused-parameter -Wno-parentheses -Wno-unknown-warning-option -pedantic -std=c++11 -fPIC -Wno-unused-local-typedefs")

execute_process (
  COMMAND llvm-config --ldflags
  OUTPUT_VARIABLE LLVM_LD_FLAGS
)
STRING(REGEX REPLACE "(\r?\n)+$" "" LLVM_LD_FLAGS "${LLVM_LD_FLAGS}")

set (CPPQUERY_LD_FLAGS "${LLVM_LD_FLAGS}")

set (CLANGLIBS clangTooling clangFrontend clangSerialization clangDriver clangRewriteCore clangParse clangSema
           clangEdit clangAnalysis clangAST clangASTMatchers clangLex clangBasic clangDynamicASTMatchers LLVMTransformUtils
           LLVMARMAsmParser LLVMX86AsmParser LLVMMipsAsmParser LLVMX86Desc LLVMSparcDesc LLVMPowerPCDesc LLVMARMDesc LLVMMipsDesc
           LLVMXCoreDesc LLVMMSP430Desc LLVMNVPTXDesc LLVMHexagonDesc LLVMX86AsmPrinter
           LLVMARMAsmPrinter LLVMMipsAsmPrinter LLVMPowerPCAsmPrinter LLVMMSP430AsmPrinter
           LLVMX86Info LLVMSparcInfo LLVMPowerPCInfo LLVMARMInfo LLVMMipsInfo LLVMXCoreInfo
           LLVMMSP430Info LLVMCppBackendInfo LLVMNVPTXInfo LLVMHexagonInfo LLVMX86Utils
           LLVMAsmParser LLVMMCParser LLVMMC LLVMCore LLVMBitWriter LLVMBitReader LLVMOption LLVMSupport)

if (ARCH STREQUAL "32")
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -m32")
  set (CPPQUERY_LD_FLAGS "${CPPQUERY_LD_FLAGS} -m32")
endif ()

if (DEBUG STREQUAL "YES")
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -g -O0")
  set (PPDEFINITIONS "${PPDEFINITIONS} -D_DEBUG")
else ()
  set (CPPQUERY_CXX_FLAGS "${CPPQUERY_CXX_FLAGS} -O3 -s")
endif ()

include_directories("include/" ${GTEST_INCLUDE_DIRS} ${CLANG_INCLUDE_DIR} ${LLVM_SRC_ROOT})

add_definitions (${PPDEFINITIONS})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPQUERY_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPPQUERY_LD_FLAGS}")

# Main executable 
add_executable(CppQuery gui/main.cpp gui/mainwindow.cpp gui/query_widget.cpp src/session.cpp)
target_link_libraries(CppQuery ${CLANGLIBS} Qt5::Widgets)

# Unit Tests
add_executable(unittest tests/automata_generator/test.cpp)
target_link_libraries(unittest ${CLANGLIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
add_test(unittest unittest)