cmake_minimum_required(VERSION 3.29)

set(KF_MIN_VERSION "6.25.0")
set(PIM_VERSION "1.0.0")
set(REQUIRED_QT_VERSION 6.9.0)

project(KAccountAutoconfig VERSION ${PIM_VERSION})

include(FeatureSummary)
find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
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
include(ECMPoQmTools)

find_package(Qt6Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
find_package(KF6CoreAddons ${KF_MIN_VERSION} CONFIG REQUIRED)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

add_subdirectory(src)

if (BUILD_TESTING)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)
    add_subdirectory(autotests)
endif()

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
