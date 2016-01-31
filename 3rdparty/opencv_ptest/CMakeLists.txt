set(target "opencv_ptest")

file(GLOB hdrs "*/*.h" "*/*.hpp")
file(GLOB srcs "*/*.cpp")

# VS2012 doesn't support correctly the tuples yet,
# see http://code.google.com/p/googletest/issues/detail?id=412
if(MSVC)
    add_definitions(/D _VARIADIC_MAX=10)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_library(${target} STATIC ${srcs})

target_link_libraries(${target})

# set(the_description "The ts module")

# if(IOS)
#   ocv_module_disable(ts)
# endif()

# set(OPENCV_MODULE_TYPE STATIC)
# set(OPENCV_MODULE_IS_PART_OF_WORLD FALSE)

# ocv_warnings_disable(CMAKE_CXX_FLAGS -Wundef)

# ocv_add_module(ts opencv_core opencv_features2d opencv_highgui opencv_imgproc opencv_video opencv_calib3d)

# ocv_glob_module_sources()
# ocv_module_include_directories()
# ocv_create_module()

# ocv_add_precompiled_headers(${the_module})
