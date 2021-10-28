cmake_minimum_required(VERSION 3.5)

set(This ExampleTest)

set(Sources
    ExampleTest.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    example
)

add_test(
    NAME ${This}
    COMMAND ${This}
)