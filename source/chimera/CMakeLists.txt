add_library(chimera_static STATIC
        bellerophon.cpp
        ccode.cpp
        chimeracheckrdp.cpp
        chimerarealigner.cpp
        chimeraslayer.cpp
        decalc.cpp
        maligner.cpp
        mothurchimera.cpp
        myPerseus.cpp
        pintail.cpp
        slayer.cpp
)

#TODO: datastructures should not be there
target_link_libraries(chimera_static "${ALL_MOTHUR_STATIC_LIBS}" datastructures_static)
