# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

set(SOURCES
    "main.cpp"
    "vulkanbase/VulkanBase.h" 
    "vulkanbase/VulkanUtil.h"
    "vulkanbase/VulkanUtil.cpp"
    # Add other source files here
    "labwork/Weeks/Week01.cpp"
    "labwork/Weeks/Week02.cpp"
    "labwork/Weeks/Week03.cpp"
    "labwork/Weeks/Week04.cpp"
    "labwork/Weeks/Week05.cpp"
    "labwork/Weeks/Week06.cpp"
 "MachineShader.h" "MachineShader.cpp" 
)

set(OBJ_FILE "${CMAKE_SOURCE_DIR}/Project/resources/lowpoly_bunny.obj")
add_custom_target(BunnyObj SOURCES ${OBJ_FILE})

# Create the execut
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES}  "labwork/Mesh.h" "labwork/Mesh.cpp" "labwork/Camera.h" "labwork/Utils.h")
add_dependencies(${PROJECT_NAME} Shaders BunnyObj)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)


configure_file(${CMAKE_SOURCE_DIR}/Project/resources/lowpoly_bunny.obj ${CMAKE_CURRENT_BINARY_DIR}/resources/lowpoly_bunny.obj COPYONLY)