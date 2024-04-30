file(GLOB_RECURSE nvvk_SRC
     "*.hpp"
     "*.cpp"
)

add_library(nvvkLib STATIC ${nvvk_SRC})
target_include_directories(nvvkLib PUBLIC .) 