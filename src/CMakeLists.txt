# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(KF6AccountAutoconfig)
add_library(KF6::AccountAutoconfig ALIAS KF6AccountAutoconfig)

generate_export_header(KF6AccountAutoconfig BASE_NAME kaccountautoconfig)

set(KAccountAutoconfig_LIB_SRCS
    lib/lookupproviderjobbase.cpp
    lib/kaccountautoconfigjob.cpp
    lib/kaccountautoconfigserver.cpp
    lib/emailproviderlookupjob.cpp
    lib/ispdbproviderlookupjob.cpp
    lib/autoconfigparser.cpp
    lib/lookuporchestratorjob.cpp
    lib/autoconfigserverhelper.cpp
    lib/localfileproviderlookupjob.cpp
    lib/emailproviderlookupjob_p.h
    lib/autoconfigparser_p.h
    lib/autoconfigserverhelper_p.h
    lib/lookupproviderjobbase_p.h
    lib/ispdbproviderlookupjob_p.h
    lib/lookuporchestratorjob_p.h
    lib/localfileproviderlookupjob_p.h
    lib/autoconfigserverhelper_p.h
    lib/mxproviderlookupjob.cpp
    lib/mxproviderlookupjob_p.h
)

target_sources(
    KF6AccountAutoconfig
    PRIVATE
        ${KAccountAutoconfig_LIB_SRCS}
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                lib
            FILES
                lib/kaccountautoconfigjob.h
                lib/kaccountautoconfigserver.h
)

target_link_libraries(
    KF6AccountAutoconfig
    PUBLIC
        KF6::CoreAddons # for KJob
    PRIVATE
        Qt6::Network
        PkgConfig::Libpsl # to extract TLD
)

set_target_properties(
    KF6AccountAutoconfig
    PROPERTIES
        VERSION
            ${KACCOUNTAUTOCONFIG_VERSION}
        SOVERSION
            ${KACCOUNTAUTOCONFIG_SOVERSION}
        EXPORT_NAME
            AccountAutoconfig
)

ecm_create_qm_loader(KF6AccountAutoconfig kaccountautoconfig6_qt)

ecm_generate_headers(KACCOUNTAUTOCONFIG_CamelCase_HEADERS
    HEADER_NAMES
    KAccountAutoconfigJob
    KAccountAutoconfigServer
    PREFIX KAccountAutoconfig
    RELATIVE lib
    REQUIRED_HEADERS KACCOUNTAUTOCONFIG_HEADERS
)

ecm_generate_export_header(KF6AccountAutoconfig
    BASE_NAME kaccountautoconfig
    VERSION ${KACCOUNTAUTOCONFIG_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)

ecm_qt_declare_logging_category(KF6AccountAutoconfig HEADER kaccountautoconfig_debug.h IDENTIFIER KACCOUNTAUTOCONFIG_LOG CATEGORY_NAME org.kde.pim.kaccountautoconfig)

install(
    TARGETS
        KF6AccountAutoconfig
    EXPORT KF6AccountAutoconfigTargets
    FILE_SET HEADERS
        DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KF6/KAccountAutoconfig/kaccountautoconfig/"
    ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    COMPONENT Devel
)

install(
    FILES
        ${KACCOUNTAUTOCONFIG_CamelCase_HEADERS}
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KF6/KAccountAutoconfig/"
    COMPONENT Devel
)

target_include_directories(
    KF6AccountAutoconfig
    INTERFACE
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KF6/KAccountAutoconfig>"
)

if(BUILD_TESTING)
    add_library(KAccountAutoconfigStatic STATIC)
    target_sources(KAccountAutoconfigStatic PRIVATE ${KAccountAutoconfig_LIB_SRCS})
    target_compile_definitions(KAccountAutoconfigStatic PUBLIC KACCOUNTAUTOCONFIG_STATIC_DEFINE=1)

    target_link_libraries(
        KAccountAutoconfigStatic
        PUBLIC
            KF6::CoreAddons # for KJob
            Qt6::Network
            PkgConfig::Libpsl
    )
    ecm_qt_declare_logging_category(KAccountAutoconfigStatic HEADER kaccountautoconfig_debug.h IDENTIFIER KACCOUNTAUTOCONFIG_LOG CATEGORY_NAME org.kde.pim.kaccountautoconfig)
endif()

if(${ECM_VERSION} VERSION_GREATER_EQUAL 6.33)
include(ECMInstalledLibraryCheck)

ecm_add_installed_library_check(KF6AccountAutoconfig
    PACKAGE_NAME "KF6AccountAutoconfig"
    PACKAGE_TARGET_NAMESPACE "KF6::"
    QT_STRICT_MODE_LEVEL 6.12
)

ecm_installed_library_check_include_strings(KF6AccountAutoconfig
    HEADERS ${KACCOUNTAUTOCONFIG_CamelCase_HEADERS}
)

ecm_installed_library_check_version_preprocessor_macros(KF6AccountAutoconfig
    PREFIX KACCOUNTAUTOCONFIG
)

endif()
