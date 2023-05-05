cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)

set(YDK_CORE_SAMPLES_DESCRIPTION "Samples")

set(samples bgp netconf)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
foreach(sample IN LISTS samples)
    add_executable(${sample} ${sample}.cpp)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${sample} ydk)

endforeach(sample)
