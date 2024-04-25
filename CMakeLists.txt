cmake_minimum_required(VERSION 3.20)

project(mothur CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BASIC_CFLAGS
        -pedantic
        -march=native
        -Wall
        -Wextra
        -Werror
        -Wcast-align
        -Wwrite-strings
        -Wformat=2
        -Wswitch-default
        -Wswitch-enum
        -Wcast-align
        -Wpointer-arith
        -Wbad-function-cast
        -Wstrict-overflow=5
        -Wstrict-prototypes
        -Wstrict-aliasing
        -Winline
        -Wundef
        -Wnested-externs
        -Wcast-qual
        -Wshadow
        -Wlogical-op
        -Wfloat-equal
        -Wredundant-decls
        -Wold-style-definition
        -fno-omit-frame-pointer
        -ffloat-store
        -fno-common
        -fstrict-aliasing
        -ggdb3
        -g
        -pipe
        -O2
)

enable_testing()

add_subdirectory(source)
