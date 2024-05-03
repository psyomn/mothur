# this section is taken from the google page for gtest + cmake
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(mothur_tests main.cpp)
target_include_directories(mothur_tests PRIVATE "${CMAKE_SOURCE_DIR}/source")
target_link_libraries(mothur_tests
        GTest::gtest_main

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
