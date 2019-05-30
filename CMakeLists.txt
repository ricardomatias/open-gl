cmake_minimum_required(VERSION 3.6)
project(opg VERSION 0.1.0)

set(CPACK_PROJECT_NAME opg)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(APP_TARGET opg)

include(CPack)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
LINK_DIRECTORIES(/usr/local/lib)

file(GLOB source_files
  "src/*.h"
  "src/*.cpp"
)

add_subdirectory("${PROJECT_SOURCE_DIR}/src/vendor/imgui")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/vendor/stb_image")

add_executable(${APP_TARGET} src/main.cpp ${source_files})
target_link_libraries(${APP_TARGET} imgui)
target_link_libraries(${APP_TARGET} stb_image)

set_target_properties (${APP_TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(${APP_TARGET} PRIVATE res)

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

target_link_libraries(
    ${APP_TARGET}
    CONAN_PKG::glfw
    CONAN_PKG::glm
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR}/bin)
