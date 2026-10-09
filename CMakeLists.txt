cmake_minimum_required(VERSION 3.16)
project(summoners_sim CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)   # new .cpp files anywhere under src/ are picked up
add_executable(sim ${SOURCES})
target_include_directories(sim PRIVATE src)
target_link_libraries(sim raylib)
if(APPLE)
  target_link_libraries(sim "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()

# copy assets next to the executable after every build
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/champions)
add_custom_command(TARGET sim POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:sim>/assets)
