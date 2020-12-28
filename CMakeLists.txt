cmake_minimum_required(VERSION 3.17)
project(Task1)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)
# Include MPI includes
include_directories(${MPI_INCLUDE_PATH})

add_executable(Task1 main.cpp main.h task1/task1.cpp task1/task1.h task2/task2.cpp task2/task2.h
        task3/task3.cpp task3/task3.h task4/task4.cpp task4/task4.h task5/task5.cpp task5/task5.h
        task6/task6.cpp task6/task6.h task7/task7.cpp task7/task7.h task8/task8.cpp task8/task8.h
        task9/task9.cpp task9/task9.h task10/task10.cpp task10/task10.h task11/task11.cpp task11/task11.h
        task12/task12.cpp task12/task12.h task13/task13.cpp task13/task13.h task14/task14.cpp task14/task14.h task16/task16.cpp task16/task16.h)

# Link against MPI
target_link_libraries(Task1 ${MPI_LIBRARIES})

if (MPI_COMPILE_FLAGS)
    set_target_properties(Task1 PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif ()
if (MPI_LINK_FLAGS)
    set_target_properties(Task1 PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif ()

