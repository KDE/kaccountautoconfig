
include(ECMAddTests)

ecm_add_tests(
        kaccountautoconfigtest.cpp
        kautoconfigservertest.cpp

        LINK_LIBRARIES KAccountAutoconfig Qt6::Test Qt6::Core
)