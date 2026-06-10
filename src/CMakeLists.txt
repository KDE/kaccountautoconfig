add_library(KAccountAutoconfig)

generate_export_header(KAccountAutoconfig)

target_sources(KAccountAutoconfig
        PRIVATE
        lib/kaccountautoconfigJob.cpp
        lib/kaccountautoconfigJob.h
        lib/kautoconfigserver.cpp
        lib/kautoconfigserver.h
)

target_link_libraries(
        KAccountAutoconfig
        KF6::CoreAddons # for KJob
)