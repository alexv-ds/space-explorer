message("----- CONFIGURING SPDLOG -----")
option(SPDLOG_FMT_EXTERNAL ON)
option(SPDLOG_FMT_EXTERNAL_HO ON)
option(SPDLOG_ENABLE_PCH ON)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../third_party/spdlog)


