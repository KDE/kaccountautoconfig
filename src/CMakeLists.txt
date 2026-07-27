# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(KAccountAutoconfig)

generate_export_header(KAccountAutoconfig)

set(KAccountAutoconfig_LIB_SRCS
    lib/lookupproviderjobbase.cpp
    lib/kaccountautoconfigjob.cpp
    lib/kautoconfigserver.cpp
    lib/emailproviderlookupjob.cpp
    lib/ispdbproviderlookupjob.cpp
    lib/autoconfigparser.cpp
    lib/lookupproviderjobbase_p.h
    lib/ispdbproviderlookupjob_p.h
    lib/lookuporchestratorjob.cpp
    lib/autoconfigserverhelper.cpp
    lib/emailproviderlookupjob_p.h
    lib/autoconfigparser_p.h
    lib/ispdbproviderlookupjob_p.h
    lib/autoconfigserverhelper_p.h
)

target_sources(
    KAccountAutoconfig
    PRIVATE
        ${KAccountAutoconfig_LIB_SRCS}
    PUBLIC
        FILE_SET HEADERS
            FILES
                lib/kaccountautoconfigjob.h
                lib/kautoconfigserver.h
)

target_link_libraries(
    KAccountAutoconfig
    PUBLIC
        KF6::CoreAddons # for KJob
        Qt6::Network
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

ecm_qt_declare_logging_category(KAccountAutoconfig HEADER kaccountautoconfig_debug.h IDENTIFIER KACCOUNTAUTOCONFIG_LOG CATEGORY_NAME org.kde.pim.kaccountautoconfig)

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

if(BUILD_TESTING)
    add_library(KAccountAutoconfigStatic STATIC)
    target_sources(KAccountAutoconfigStatic PRIVATE ${KAccountAutoconfig_LIB_SRCS})

    target_link_libraries(
        KAccountAutoconfigStatic
        PUBLIC
            KF6::CoreAddons # for KJob
            Qt6::Network
    )
    # Needed for windows dll linkage error on static
    set_target_properties(
        KAccountAutoconfigStatic
        PROPERTIES
            COMPILE_FLAGS
                -DKACCOUNTAUTOCONFIG_STATIC_DEFINE=1
    )
    ecm_qt_declare_logging_category(KAccountAutoconfigStatic HEADER kaccountautoconfig_debug.h IDENTIFIER KACCOUNTAUTOCONFIG_LOG CATEGORY_NAME org.kde.pim.kaccountautoconfig)
endif()
