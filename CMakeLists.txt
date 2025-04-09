cmake_minimum_required(VERSION 3.12)
project(KrillKounter VERSION 0.3.0)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wshadow=local")

# Find packages and libraries
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(JSONGLIB REQUIRED json-glib-1.0)

# Testing
include(CTest)
enable_testing()

# List of all source dirs and formats
file(GLOB sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cc
)

# Shared Library
add_library(krillkounter SHARED ${sources})

target_include_directories(krillkounter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link libraries
target_link_libraries(krillkounter PUBLIC ${JSONGLIB_LIBRARIES})
target_link_libraries(krillkounter PUBLIC ${CMAKE_DL_LIBS})

set_target_properties(krillkounter PROPERTIES
        LINKER_LANGUAGE CXX
        VERSION ${CMAKE_PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)


add_executable(KrillKounter src/main.cc)

# Include directories
include_directories(${JSONGLIB_INCLUDE_DIRS})

target_include_directories(KrillKounter PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(KrillKounter PUBLIC ${JSONGLIB_LIBRARIES})

target_link_libraries(KrillKounter PUBLIC ${CMAKE_DL_LIBS})

target_link_libraries(KrillKounter PUBLIC krillkounter)

# We link C++ files
set_target_properties(KrillKounter PROPERTIES LINKER_LANGUAGE CXX)


# Project metadata
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(DIRECTORY DESTINATION  /usr/share/KrillKounter)
install(TARGETS KrillKounter RUNTIME DESTINATION /usr/bin)

install(TARGETS krillkounter LIBRARY DESTINATION /usr/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/library/
        DESTINATION /usr/include/KrillKounter
        FILES_MATCHING PATTERN "*.hh"
)

install(FILES src/daemon/service/KrillKounter.service DESTINATION /lib/systemd/system)
