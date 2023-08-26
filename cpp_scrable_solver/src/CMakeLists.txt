find_package(unordered_dense CONFIG REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -Wall")

add_library(Scrabble Scrabble.h Scrabble.cpp Repertoire.h Repertoire.cpp)

target_link_libraries(Scrabble unordered_dense::unordered_dense)
