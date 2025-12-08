# set the CMake and C++ version/standard numbers
cmake_minimum_required(VERSION 3.25)
project(cpp_sandbox LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CTest
include(CTest)

# skeleton's default dependencies
# libs will be discoverable via find_package once vcpkg's toolchain is active
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
  add_link_options(-fsanitize=address,undefined)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

# create an executable called "app", using src/main.cpp
add_executable(app src/main.cpp)
# libraries to link
target_link_libraries(app PRIVATE
  spdlog::spdlog
  nlohmann_json::nlohmann_json
  asio::asio
  Threads::Threads
)
