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

ecm_add_test(
    fakeservertest.cpp
    fakeserver/stubnetworkaccessmanager.cpp

    TEST_NAME fakeservertest

    LINK_LIBRARIES
    Qt6::Test
    Qt6::Network
)

ecm_add_test(
    emailproviderlookupjobtest.cpp
    fakeserver/stubnetworkaccessmanager.cpp

    TEST_NAME emailproviderlookupjobtest

    LINK_LIBRARIES
    KAccountAutoconfigStatic
    Qt6::Test
    Qt6::Network
)

ecm_add_test(
    ispdbproviderlookupjobtest.cpp
    fakeserver/stubnetworkaccessmanager.cpp

    TEST_NAME ispdbproviderlookupjobtest

    LINK_LIBRARIES
    KAccountAutoconfigStatic
    Qt6::Test
    Qt6::Network
)

ecm_add_test(
    lookuporchestratorjobtest.cpp
    fakeserver/stubnetworkaccessmanager.cpp

    TEST_NAME lookuporchestratorjobtest

    LINK_LIBRARIES
    KAccountAutoconfigStatic
    Qt6::Test
    Qt6::Network
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
target_compile_definitions(emailproviderlookupjobtest PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_compile_definitions(ispdbproviderlookupjobtest PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_compile_definitions(lookuporchestratorjobtest PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
