cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)

set(YDK_CORE_SAMPLES_DESCRIPTION "Samples")

set(samples bgp)
link_libraries(${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${Boost_LIBRARIES})
foreach(sample IN LISTS samples)
    add_executable(${sample} ${sample}.cpp)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_definitions(${sample} PRIVATE "BOOST_ALL_DYN_LINK=1")
    target_compile_definitions(${sample} PRIVATE "BOOST_LOG_DYN_LINK=1")
    #target_include_directories(${sample} PRIVATE ${BOOST_INCLUDE_DIRS})
    target_link_libraries(${sample} ydk)
    #target_link_libraries(${sample} ydk         ${Boost_LOG_SETUP_LIBRARY}
    #    ${Boost_LOG_LIBRARY}
    #    ${BOOST_THREAD_LIBRARY}
    #    ${BOOST_DATE_TIME_LIBRARY}
    #    ${Boost_FILESYSTEM_LIBRARY}
    #    ${Boost_SYSTEM_LIBRARY}
    #    ${Boost_LOG_LIBRARY}
    #	${xml2_location}
    #	${curl_location}
    #	${ssh_threads_location}
    #    ${pcre_location}
    #	${xslt_location}
    #    ${LIBSSH_LIBRARIES})

endforeach(sample)

