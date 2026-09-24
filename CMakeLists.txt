cmake_minimum_required(VERSION 3.21)

project(
    ligarium
    VERSION 0.1.0
    DESCRIPTION "A C++/Qt 6 ORM for mapping records and relationships to SQL databases"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)


# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

file(GLOB_RECURSE LIGARIUM_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/field.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ligarium.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/link.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/database.cpp"
)

file(GLOB_RECURSE LIGARIUM_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/field.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/schema.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ligarium.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/database.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/link.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/polymorphic_link.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/record.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/record_registry.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_registry.h"
)

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------


add_library(ligarium ${LIGARIUM_SOURCES})

add_library(ligarium::ligarium ALIAS ligarium)

target_sources(ligarium
    PRIVATE
        ${LIGARIUM_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            ${LIGARIUM_HEADERS}
)

target_include_directories(ligarium
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ligarium
    PUBLIC
        Qt6::Core
        Qt6::Sql
        Qt6::Widgets
)

target_compile_features(ligarium
    PUBLIC
        cxx_std_20
)

set_target_properties(ligarium PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME ligarium
)



# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

install(
    TARGETS ligarium
    EXPORT ligariumTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS
)

install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        "${CMAKE_CURRENT_SOURCE_DIR}/NOTICE"
    DESTINATION
        ${CMAKE_INSTALL_DOCDIR}
)

# ---------------------------------------------------------------------------
# CMake package
# ---------------------------------------------------------------------------

install(
    EXPORT ligariumTargets
    FILE ligariumTargets.cmake
    NAMESPACE ligarium::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ligarium
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LigariumConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LigariumConfig.cmake"
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/ligarium
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LigariumConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/LigariumConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LigariumConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/ligarium
)



option(LIGARIUM_BUILD_TESTS
    "Build ligarium tests"
    OFF
)

if(LIGARIUM_BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    add_subdirectory(tests)


    target_compile_options(ligarium
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-include${CMAKE_CURRENT_SOURCE_DIR}/tests/ligarium_config.h>
        $<$<CXX_COMPILER_ID:MSVC>:/FI${CMAKE_CURRENT_SOURCE_DIR}/tests/ligarium_config.h>
    )


    enable_testing()


endif()