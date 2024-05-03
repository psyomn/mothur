# TODO: this should be eventually removed
include_directories("${CMAKE_SOURCE_DIR}/source")

add_subdirectory(calculators)
add_subdirectory(chimera)
add_subdirectory(classifier)
add_subdirectory(clearcut)
add_subdirectory(commands)
add_subdirectory(communitytype)
add_subdirectory(datastructures)
add_subdirectory(engines)
add_subdirectory(metastats)
add_subdirectory(read)
add_subdirectory(svm)
add_subdirectory(uchime_src)

add_library(mothur_static STATIC
        alignreport.cpp
        averagelinkage.cpp
        calcsparcc.cpp
        clusterclassic.cpp
        cluster.cpp
        collect.cpp
        commandfactory.cpp
        commandoptionparser.cpp
        completelinkage.cpp
        consensus.cpp
        currentfile.cpp
        dlibshuff.cpp
        fileoutput.cpp
        gotohoverlap.cpp
        heatmap.cpp
        heatmapsim.cpp
        inputdata.cpp
        libshuff.cpp
        linearalgebra.cpp
        mothurout.cpp
        myseqdist.cpp
        nast.cpp
        needlemanoverlap.cpp
        noalign.cpp
        opticluster.cpp
        optifitcluster.cpp
        optionparser.cpp
        overlap.cpp
        raredisplay.cpp
        rarefact.cpp
        refchimeratest.cpp
        sensspeccalc.cpp
        seqnoise.cpp
        singlelinkage.cpp
        slibshuff.cpp
        subsample.cpp
        summary.cpp
        trialSwap2.cpp
        trimoligos.cpp
        utils.cpp
        validcalculator.cpp
        validparameter.cpp
        venn.cpp
        vsearchfileparser.cpp
        weightedlinkage.cpp
        wilcox.cpp
)

target_link_libraries(mothur_static
        commands_static
        datastructures_static
        calculators_static
        chimera_static
        classifier_static
        clearcut_static
        communitytype_static
        engines_static
        metastats_static
        read_static
        svm_static
        uchime_static
)

set(ALL_MOTHUR_STATIC_LIBS
        commands_static
        datastructures_static
        calculators_static
        chimera_static
        classifier_static
        clearcut_static
        communitytype_static
        engines_static
        metastats_static
        read_static
        svm_static
        uchime_static
        mothur_static
)

add_executable(mothur mothur.cpp)
target_link_libraries(mothur "${ALL_MOTHUR_STATIC_LIBS}")
