set(target ${PERF_TESTS})

file(GLOB hdrs "*.h*")
file(GLOB srcs "*.cpp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

# VS2012 doesn't support correctly the tuples yet,
# see http://code.google.com/p/googletest/issues/detail?id=412
if(MSVC)
    add_definitions(/D _VARIADIC_MAX=10)
endif()

add_executable(${target} ${srcs} ${hdrs})
target_link_libraries(${target} gtest ${LIBRARY})

add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/testdata" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/testdata"
)
