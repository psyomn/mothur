add_library(clearcut_static STATIC
        clearcut.cpp
        cmdargs.cpp
        distclearcut.cpp
        dmat.cpp
        fasta.cpp
        getopt_long.cpp
)

target_link_libraries(clearcut_static "${ALL_MOTHUR_STATIC_LIBS}" calculators_static)
