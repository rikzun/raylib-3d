cmake_minimum_required(VERSION 3.29)
if(WIN32)
    execute_process(COMMAND chcp 65001)
endif()
include(CMakeDeclares)

project(mrrr)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(NOTICE "🔷 load raylib")
FetchContent_MakeAvailable(raylib)
message(NOTICE "✅ raylib loaded")

message(NOTICE "🔷 load tracy")
FetchContent_MakeAvailable(tracy)
message(NOTICE "✅ tracy loaded")

file(GLOB project_source_files "src/*.cpp")
add_executable(mrrr ${project_source_files})
target_include_directories(mrrr PRIVATE ${CMAKE_SOURCE_DIR}/includes)

target_link_libraries(mrrr raylib)

target_compile_definitions(mrrr PRIVATE TRACY_ENABLE)
target_compile_definitions(mrrr PRIVATE TRACY_ONLY_LOCALHOST)
target_include_directories(mrrr PRIVATE ${tracy_SOURCE_DIR}/public/tracy)
target_sources(mrrr PRIVATE ${tracy_SOURCE_DIR}/public/TracyClient.cpp)

add_custom_command(
    TARGET mrrr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:mrrr>/assets
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(mrrr PRIVATE DEBUG_BUILD)
else()
    target_compile_definitions(mrrr PRIVATE RELEASE_BUILD)
endif()

if(WIN32)
    target_compile_definitions(mrrr PRIVATE OS_WINDOWS)
elseif(APPLE)
    target_compile_definitions(mrrr PRIVATE OS_MACOS)
elseif(UNIX)
    target_compile_definitions(mrrr PRIVATE OS_LINUX)
endif()