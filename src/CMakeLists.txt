add_library(KAccountAutoconfig)

generate_export_header(KAccountAutoconfig)

target_sources(KAccountAutoconfig
    PRIVATE
        lib/kaccountautoconfigJob.cpp
        lib/kaccountautoconfigJob.h
    PUBLIC
        FILE_SET HEADERS
        FILES lib/kaccountautoconfigJob.h
)

target_link_libraries(
    KAccountAutoconfig
    KF6::CoreAddons # for KJob
)