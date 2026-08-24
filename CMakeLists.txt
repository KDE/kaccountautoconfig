# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.29)

set(KF_MIN_VERSION "6.25.0")
set(PIM_VERSION "1.0.0")
set(REQUIRED_QT_VERSION 6.9.0)

project(KAccountAutoconfig VERSION ${PIM_VERSION})

include(FeatureSummary)
find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(
    ECM
    PROPERTIES
        TYPE
            REQUIRED
        DESCRIPTION
            "Extra CMake Modules."
        URL
            "https://commits.kde.org/extra-cmake-modules"
)
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMGenerateQDoc)

include(ECMGenerateExportHeader)
include(ECMGeneratePkgConfigFile)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)

find_package(Qt6Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Network)
find_package(KF6CoreAddons ${KF_MIN_VERSION} CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Libpsl REQUIRED IMPORTED_TARGET libpsl)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT
    0
    CACHE STRING
    "Control the range of deprecated API excluded from the build [default=0]."
)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX KACCOUNTAUTOCONFIG
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/kaccountautoconfig_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF6AccountAutoconfigConfigVersion.cmake"
    SOVERSION 6
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/src/kaccountautoconfig_version.h"
        "${CMAKE_CURRENT_BINARY_DIR}/src/kaccountautoconfig_export.h"
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KF6/KAccountAutoconfig/kaccountautoconfig/"
    COMPONENT Devel
)

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)
    add_subdirectory(autotests)
endif()

include(ECMFeatureSummary)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6AccountAutoconfig")

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KF6AccountAutoconfigConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KF6AccountAutoconfigConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KAccountAutoconfigConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6AccountAutoconfigConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

install(
    EXPORT KF6AccountAutoconfigTargets
    NAMESPACE KF6::
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KF6AccountAutoconfigTargets.cmake
)
