cmake_minimum_required(VERSION 3.0.0)
project(opg VERSION 0.1.0)

set(CPACK_PROJECT_NAME opg)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
LINK_DIRECTORIES(/usr/local/lib)

file(GLOB source_files
  "src/*.h"
  "src/*.cpp"
  "src/vendor/**/*.h"
  "src/vendor/**/*.cpp"
)

add_executable(opg src/main.cpp ${source_files})
set_target_properties (opg PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(opg PRIVATE res)

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

target_link_libraries(
    opg
    CONAN_PKG::glfw
    CONAN_PKG::glm
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR}/bin)
