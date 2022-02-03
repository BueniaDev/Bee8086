project(Bee8086-SDL2)

option(BUILD_KUJOBIOS "Build the custom-built KujoBIOS." OFF)

# Require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (BUILD_KUJOBIOS STREQUAL "ON")
    add_custom_target(kujobios ALL
	COMMAND nasm ${CMAKE_CURRENT_SOURCE_DIR}/bios_source/kujobios.asm -o ${CMAKE_BINARY_DIR}/kujobios.bin)
endif()

set(EXAMPLE_SOURCES
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_MAIN_HANDLED")

add_executable(Bee8086-SDL2 ${EXAMPLE_SOURCES})
target_include_directories(Bee8086-SDL2 PUBLIC ${BEE8086_INCLUDE_DIR})
target_link_libraries(Bee8086-SDL2 libbee8086)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (TARGET SDL2::SDL2)
    target_link_libraries(Bee8086-SDL2 SDL2::SDL2)
else()
    target_link_libraries(Bee8086-SDL2 ${SDL2_LIBRARIES})
endif()

if (APPLE)
    set_target_properties(Bee8086-SDL2 PROPERTIES
                         MACOSX_BUNDLE true
                         MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/bee8086-frontend.plist.in
                         OUTPUT_NAME Bee8086-SDL2)
endif()

