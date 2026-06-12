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
