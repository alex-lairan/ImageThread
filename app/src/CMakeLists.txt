SET(GCC_COVERAGE_COMPILE_FLAGS "-Ofast -ftree-vectorize -fopt-info -mavx2 -fopt-info-vec-all -ftree-vectorizer-verbose=0 -fopenmp")

include_directories(${ImageThread_SOURCE_DIR}/app/include)

include_directories(${ImageThread_SOURCE_DIR}/lib/bitmap)
link_directories(${ImageThread_SOURCE_DIR}/lib/bitmap)

add_executable(image_thread main.c effects.c stack_image.c)
target_link_libraries(image_thread Bitmap m pthread)

install(TARGETS image_thread DESTINATION bin)
