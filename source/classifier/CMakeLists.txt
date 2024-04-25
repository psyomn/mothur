add_library(classifier_static STATIC
        alignnode.cpp
        aligntree.cpp
        bayesian.cpp
        classify.cpp
        kmernode.cpp
        kmertree.cpp
        knn.cpp
        phylosummary.cpp
        phylotree.cpp
        taxonomyequalizer.cpp
        taxonomynode.cpp
)

target_include_directories(classifier_static
        PRIVATE
                "${CMAKE_SOURCE_DIR}/source/"
)
