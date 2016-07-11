set (CMAKE_CXX_STANDARD 11)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

file(GLOB_RECURSE SOURCES "*.cpp")

include_directories(
.
./core/
./core/cpu/
)

find_package(SDL2)

add_executable(GamePerson ${SOURCES})
target_link_libraries(GamePerson SDL2 pthread GL)
