cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(SudokuSolver)

# Establecer el estándar de C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir el ejecutable
add_executable(SudokuSolver sudoku_solver.cpp)

# Vincular OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(SudokuSolver PUBLIC OpenMP::OpenMP_CXX)
endif()

# Vincular JsonCpp
find_package(JsonCpp REQUIRED)
target_link_libraries(SudokuSolver PUBLIC JsonCpp::JsonCpp)
