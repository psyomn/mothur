add_library(clearcut_static STATIC
        clearcut.cpp
        cmdargs.cpp
        distclearcut.cpp
        dmat.cpp
        fasta.cpp
        getopt_long.cpp
)

target_include_directories(clearcut_static
        PRIVATE
                "${CMAKE_SOURCE_DIR}/source/"
)
