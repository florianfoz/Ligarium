set(CMAKE_AUTOMOC ON)

function(add_ligarium_test TEST_NAME)
    add_executable(${TEST_NAME}
        ${ARGN}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
            ligarium::ligarium
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
endfunction()

add_ligarium_test(test_all
    test_all.cpp
    test_widget_registry.h
    
    records.h
    records.cpp
)

add_ligarium_test(test_database
    test_database.cpp
)

add_ligarium_test(test_link
    test_link.cpp
)

add_ligarium_test(test_polymorphic_link
    test_polymorphic_link.cpp
)

add_ligarium_test(test_records
    test_records.cpp
    records.h
    records.cpp
)

add_ligarium_test(test_widget_registry
    test_widget_registry.cpp
    test_widget_registry.h
)

add_ligarium_test(test_association_record
    test_association_record.cpp
)

add_ligarium_test(ligarium_demo

    test_ui.cpp
    test_ui.h

    w_ligarium_main.cpp
    w_ligarium_main.h
    w_ligarium_main.ui

    w_property.cpp
    w_property.h
    w_property.ui

    w_tenant.cpp
    w_tenant.h
    w_tenant.ui

    w_tenant_property.cpp
    w_tenant_property.h
    w_tenant_property.ui

    records.h
    records.cpp

)
