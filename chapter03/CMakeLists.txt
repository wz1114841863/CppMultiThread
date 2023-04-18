FILE(GLOB source_file CONFIGURE_DEPENDS ./*.cpp)
FILE(GLOB source_head_file CONFIGURE_DEPENDS ./*.h)
add_library(chap03 ${source_file} ${source_head_file})
target_include_directories(chap03 PUBLIC .)