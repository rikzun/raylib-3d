cmake_minimum_required(VERSION 3.29)
include(CMakeDeclares)

project(vmm VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(NOTICE "🔷 load SDL")
FetchContent_MakeAvailable(SDL)
message(NOTICE "✅ SDL loaded")

message(NOTICE "🔷 load glm")
FetchContent_MakeAvailable(glm)
message(NOTICE "✅ glm loaded")

# message(NOTICE "🔷 load tracy")
# FetchContent_MakeAvailable(tracy)
# message(NOTICE "✅ tracy loaded")

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADER_DIRS "src/*.h")

set(INCLUDE_DIRS "")
foreach(HEADER ${HEADER_PATHS})
    get_filename_component(DIR ${HEADER} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

add_executable(vmm ${PROJECT_SOURCE_FILES})
target_include_directories(vmm PRIVATE ${INCLUDE_DIRS})

find_package(Vulkan REQUIRED)
target_link_libraries(vmm PRIVATE SDL3::SDL3-static Vulkan::Vulkan glm)

# target_compile_definitions(vmm PRIVATE TRACY_ENABLE)
# target_compile_definitions(vmm PRIVATE TRACY_ONLY_LOCALHOST)
# target_include_directories(vmm PRIVATE ${tracy_SOURCE_DIR}/public/tracy)
# target_sources(vmm PRIVATE ${tracy_SOURCE_DIR}/public/TracyClient.cpp)

# add_custom_command(
#     TARGET vmm POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:vmm>/shaders
# )

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    set_target_properties(vmm PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

target_compile_definitions(vmm PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
target_compile_definitions(vmm PRIVATE PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(vmm PRIVATE PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(vmm PRIVATE PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(vmm PRIVATE DEBUG_BUILD)
else()
    target_compile_definitions(vmm PRIVATE RELEASE_BUILD)
endif()

if(WIN32)
    target_compile_definitions(vmm PRIVATE WINDOWS_BUILD)
elseif(APPLE)
    target_compile_definitions(vmm PRIVATE MACOS_BUILD)
elseif(UNIX)
    target_compile_definitions(vmm PRIVATE LINUX_BUILD)
endif()