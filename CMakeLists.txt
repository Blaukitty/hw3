cmake_minimum_required(VERSION 3.16)
project(ShannonCoder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(shannon STATIC
    encoder.cpp
    decoder.cpp
)

target_include_directories(shannon PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(shannon_cli cli.cpp)
target_link_libraries(shannon_cli PRIVATE shannon)

option(SHANNON_BUILD_TESTS "Build unit tests" ON)

if (SHANNON_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.0        # актуальная стабильная
    )
    FetchContent_MakeAvailable(catch2)

    add_executable(shannon_tests tests.cpp)
    target_link_libraries(shannon_tests PRIVATE shannon Catch2::Catch2WithMain)

    enable_testing()
    add_test(NAME shannon_unit COMMAND shannon_tests)
endif()
