cmake_minimum_required(VERSION 3.21)

project(
    Ligarium
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

set(ENUMLITE_BUILD_QT ON CACHE BOOL "Build enumlite Qt backend") # HERE

add_subdirectory(
    external/enumlite
)

add_library(Ligarium ${LIGARIUM_SOURCES})

add_library(Ligarium::Ligarium ALIAS Ligarium)

target_sources(Ligarium
    PRIVATE
        ${LIGARIUM_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            ${LIGARIUM_HEADERS}
)

target_include_directories(Ligarium
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(Ligarium
    PUBLIC
        Qt6::Core
        Qt6::Sql
        Qt6::Widgets
        enumlite::enumlite_qt
)

target_compile_features(Ligarium
    PUBLIC
        cxx_std_20
)

set_target_properties(Ligarium PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME Ligarium
)



# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

install(
    TARGETS Ligarium
    EXPORT LigariumTargets
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
    EXPORT LigariumTargets
    FILE LigariumTargets.cmake
    NAMESPACE Ligarium::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Ligarium
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LigariumConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LigariumConfig.cmake"
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Ligarium
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
        ${CMAKE_INSTALL_LIBDIR}/cmake/Ligarium
)



option(LIGARIUM_BUILD_TESTS
    "Build Ligarium tests"
    OFF
)

if(LIGARIUM_BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    target_compile_options(Ligarium
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-include${CMAKE_CURRENT_SOURCE_DIR}/tests/ligarium_config.h>
        $<$<CXX_COMPILER_ID:MSVC>:/FI${CMAKE_CURRENT_SOURCE_DIR}/tests/ligarium_config.h>
    )


    enable_testing()
    add_subdirectory(tests)
endif()