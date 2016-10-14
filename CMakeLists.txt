cmake_minimum_required(VERSION 3.5)
project(fastrtps_introspection)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

find_package(fastrtps_cmake_module REQUIRED)
find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)
message("FastRTPS include ${fastrtps_INCLUDE_DIR}")
message("FastRTPS libraries  ${fastrtps_LIBS}")

include_directories(${fastrtps_INCLUDE_DIR})

add_library(msgs
            idl/string.cxx
            idl/stringPubSubTypes.cxx
            idl/int.cxx
            idl/intPubSubTypes.cxx
            )
target_link_libraries(msgs
            fastrtps
            fastcdr
            )

add_executable(my_string_talker
              src/string_talker.cpp
              )
target_link_libraries(my_string_talker
                  fastrtps
                  fastcdr
                  msgs
                  )
add_executable(my_int_talker
              src/int_talker.cpp
              )
target_link_libraries(my_int_talker
                  fastrtps
                  fastcdr
                  msgs
                  )
add_executable(my_string_listener
              src/string_listener.cpp )
target_link_libraries(my_string_listener
                  fastrtps
                  fastcdr
                  msgs
                  )
add_executable(fastrtps_info
              src/fastrtps_info.cpp )
target_link_libraries(fastrtps_info
                  fastrtps
                  fastcdr
                  msgs
                  )
