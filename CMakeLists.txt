cmake_minimum_required(VERSION 3.20)
project(mentals)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_program(MOLD "mold")
if (NOT DEFINED MOLD)
    message(FATAL_ERROR "mold linker not found")
else()
    set(CMAKE_CXX_FLAGS "-fuse-ld=mold")
    message(STATUS "linker: mold")
endif()
# nvim/vscode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_PREFIX_PATH "./lib/toml++")
list(APPEND CMAKE_PREFIX_PATH "./lib/fmt")
list(APPEND CMAKE_PREFIX_PATH "./lib/liboai")
list(APPEND CMAKE_PREFIX_PATH "./lib/libpqxx")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
endif()

# Find packages
find_package(PkgConfig REQUIRED)
find_package(fmt REQUIRED)
find_package(CURL REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(nlohmann_json REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)  # Use pkg_check_modules to find Poppler

# Add source files
file(GLOB SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include /usr/local/include)
target_include_directories(${PROJECT_NAME} PRIVATE lib ${POPPLER_INCLUDE_DIRS})

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt
  CURL::libcurl
  nlohmann_json::nlohmann_json
  oai
  ${POPPLER_LIBRARIES} 
  -pthread 
  pqxx  
  pq  
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result)
  if(result)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "IPO is not supported: ${result}")
  endif()
endif()

function(copy_after_build TARGET_NAME)
  set(DESTDIR ${CMAKE_CURRENT_LIST_DIR}/bin/)
  file(MAKE_DIRECTORY ${DESTDIR})
  add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${DESTDIR}
  )
endfunction()

copy_after_build(${PROJECT_NAME})
