project(PackageTest CXX)
cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS
    system
)

enable_testing()

add_executable(test_example test_example.cpp)
target_link_libraries(test_example
    CONAN_PKG::ma_asio_samples
    Boost::system
    Threads::Threads
)
set_target_properties(test_example
    PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH True
)
add_test(
    NAME test_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
    COMMAND test_example
)
