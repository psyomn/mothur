add_library(engines_static STATIC
        batchengine.cpp
        interactengine.cpp
        scriptengine.cpp
)

target_link_libraries(engines_static mothur_static)
