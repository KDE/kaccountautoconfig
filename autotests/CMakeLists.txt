# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
include(ECMAddTests)

# self test on stub
ecm_add_test(
    fakeservertest.cpp
    fakeserver/stubnetworkaccessmanager.cpp

    TEST_NAME fakeservertest

    LINK_LIBRARIES
    Qt6::Test
    Qt6::Network
)

# unit test
macro(ACCOUNTAUTOCONFIG_UNIT_TESTS)
    foreach(_testname ${ARGN})
        ecm_add_test(
            ${_testname}.cpp
            fakeserver/stubnetworkaccessmanager.cpp

            TEST_NAME ${_testname}

            LINK_LIBRARIES
            KAccountAutoconfigStatic
            Qt6::Network
            Qt6::Test
        )
        target_compile_definitions(${_testname} PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
    endforeach()
endmacro()

accountautoconfig_unit_tests(
    autoconfigparsertest
    clientconfigstructtest
    emailproviderlookupjobtest
    ispdbproviderlookupjobtest
    kaccountautoconfigtest
    kautoconfigservertest
    localfileproviderlookupjobtest
)
