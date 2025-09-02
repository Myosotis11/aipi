sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(
    mcp_server.c
    user_mcp_tools.c
)