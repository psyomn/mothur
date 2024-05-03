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

target_link_libraries(classifier_static "${ALL_MOTHUR_STATIC_LIBS}" datastructures_static calculators_static)

