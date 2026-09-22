# CMake

Ligarium uses modern CMake and exports the target:

```cmake
Ligarium::Ligarium
```

## Requirements

* CMake 3.21+
* C++20
* Qt 6
* Qt Core
* Qt SQL

## Building

Configure:

```bash
cmake --preset debug-linux
```

Build:

```bash
cmake --build --preset build-debug-linux
```

## Tests

Tests are controlled by:

```cmake
LIGARIUM_BUILD_TESTS
```

Enable them in the preset:

```json
{
    "LIGARIUM_BUILD_TESTS": "ON"
}
```

Then:

```bash
cmake --preset debug-linux
cmake --build --preset build-debug-linux
ctest --test-dir build/debug --output-on-failure
```

## Linking

Applications link against:

```cmake
target_link_libraries(MyApplication
    PRIVATE
        Ligarium::Ligarium
)
```

Qt dependencies are propagated through the Ligarium target.

## Application configuration

Because Ligarium uses an application-defined `Table` type, the application's configuration header must be visible while compiling the Ligarium target.

For GCC and Clang:

```cmake
target_compile_options(Ligarium
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            -include${APPLICATION_LIGARIUM_CONFIG}
        >
)
```

For MSVC:

```cmake
target_compile_options(Ligarium
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /FI${APPLICATION_LIGARIUM_CONFIG}
        >
)
```

The configuration header defines:

```cpp
#define LIGARIUM_TABLE_TYPE Ligarium::Table
```

## Tests target generation

The test suite can automatically discover test sources:

```cmake
file(GLOB_RECURSE LIGARIUM_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(TEST_SOURCE ${LIGARIUM_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
            Ligarium::Ligarium
            Qt6::Test
    )

    target_compile_features(${TEST_NAME}
        PRIVATE
            cxx_std_20
    )

    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
    )
endforeach()
```

## AUTOMOC

QtTest classes using `Q_OBJECT` require AUTOMOC.

When tests are enabled:

```cmake
set(CMAKE_AUTOMOC ON)
```

This allows source files containing:

```cpp
QTEST_MAIN(TestDatabase)
#include "test_database.moc"
```

to be processed correctly.
