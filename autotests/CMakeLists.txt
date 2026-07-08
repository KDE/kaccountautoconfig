# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
include(ECMAddTests)

ecm_add_tests(
    kaccountautoconfigtest.cpp
    kautoconfigservertest.cpp

    LINK_LIBRARIES
    KAccountAutoconfig
    Qt6::Test
)

ecm_add_tests(
        clientconfigstructtest.cpp

        LINK_LIBRARIES
        KAccountAutoconfigStatic
        Qt6::Test
)

ecm_add_tests(
        autoconfigparsertest.cpp

        LINK_LIBRARIES
        KAccountAutoconfigStatic
        Qt6::Test
)

target_compile_definitions(autoconfigparsertest PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
