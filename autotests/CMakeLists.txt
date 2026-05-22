
include(ECMAddTests)

ecm_add_tests(
    kaccountautoconfigtest.cpp

    LINK_LIBRARIES
        KAccountAutoconfig
        Qt6::Test
)