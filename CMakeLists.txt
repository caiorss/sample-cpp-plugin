cmake_minimum_required(VERSION 3.9)
project(Demo_cpp_plugin)
set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)

#========== Targets Configurations ============#

# >>> ---- Target: main ----------------
add_executable(main main.cpp)
target_include_directories(main PUBLIC ".")
# Note: the library ldl is necessary for the UNIX APIs dlopen,
# dlclose, ... and so on
target_link_libraries(main PRIVATE dl)
set_target_properties(main PROPERTIES
  SUFFIX ".bin"
  )


# >>> ---- Target: PluginA ----------------
add_library(PluginA SHARED PluginA.cpp)
target_include_directories(PluginA PUBLIC ".")
set_target_properties(PluginA PROPERTIES
  PREFIX ""
  )

install( TARGETS main PluginA
         DESTINATION "./"
  )
