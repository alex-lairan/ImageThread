SET(GCC_COVERAGE_COMPILE_FLAGS "-O2 -ftree-vectorize -fopt-info -mavx2 -fopt-info-vec-all")

include_directories(${ImageThread_SOURCE_DIR}/app/include)

include_directories(${ImageThread_SOURCE_DIR}/lib/bitmap)
link_directories(${ImageThread_SOURCE_DIR}/lib/bitmap)

add_executable(MainProject main.c effects.c)
target_link_libraries(MainProject Bitmap m pthread)
