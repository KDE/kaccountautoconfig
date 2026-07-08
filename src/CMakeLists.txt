# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(KAccountAutoconfig)

generate_export_header(KAccountAutoconfig)

target_sources(
    KAccountAutoconfig
    PRIVATE
        lib/kaccountautoconfigjob.cpp
        lib/kaccountautoconfigjob.h
        lib/kautoconfigserver.h
        lib/kautoconfigserver.cpp
        lib/autoconfigparser.cpp
        lib/autoconfigparser_p.h
    PUBLIC
        FILE_SET HEADERS
            FILES
                lib/kaccountautoconfigjob.h
                lib/autoconfigparser_p.h
)

target_link_libraries(
    KAccountAutoconfig
    PUBLIC
        KF6::CoreAddons # for KJob
)

set_target_properties(
    KAccountAutoconfig
    PROPERTIES
        VERSION
            ${KACCOUNTAUTOCONFIG_VERSION}
        SOVERSION
            ${KACCOUNTAUTOCONFIG_SOVERSION}
        EXPORT_NAME
            KACCOUNTAUTOCONFIG
)

ecm_generate_headers(KACCOUNTAUTOCONFIG_CamelCase_HEADERS
    HEADER_NAMES
    KAccountAutoconfigJob
    KAutoconfigServer
    PREFIX KACCOUNTAUTOCONFIG
    RELATIVE lib
    REQUIRED_HEADERS KACCOUNTAUTOCONFIG_HEADERS
)

ecm_generate_export_header(KAccountAutoconfig
    BASE_NAME kaccountautoconfig
    VERSION ${KACCOUNTAUTOCONFIG_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)

install(
    FILES
        lib/kaccountautoconfigjob.h
        lib/kautoconfigserver.h
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KAccountAutoconfig/"
    COMPONENT Devel
)

install(
    FILES
        ${KACCOUNTAUTOCONFIG_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccountAutoconfig
    COMPONENT Devel
)
