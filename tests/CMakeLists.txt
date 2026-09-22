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