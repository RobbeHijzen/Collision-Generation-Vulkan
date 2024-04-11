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
    "VulkanBase/VulkanBase.h" 
    "VulkanUtil/VulkanUtil.h"
    "VulkanUtil/VulkanUtil.cpp"
    # Add other source files here
    "Abstraction/Weeks/Week01.cpp"
    "Abstraction/Weeks/Week02.cpp"
    "Abstraction/Weeks/Week03.cpp"
    
    
    "Abstraction/Shaders/DerivedShaders/MachineShader.h"
    "Abstraction/Shaders/DerivedShaders/MachineShader.cpp" 
    "VulkanBase/Implementations/Initialization/InstanceInit.cpp"
    "VulkanBase/Implementations/Initialization/PhysicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/LogicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/SwapChainInit.cpp"

 "Abstraction/Scene/Scene.h" "Abstraction/Scene/Scenemanager.h" "Abstraction/Scene/Scene.cpp" "Abstraction/Singleton.h" "Abstraction/Shaders/ShaderManager.h" "Abstraction/Shaders/Shader.h"  "Abstraction/VertexInfo.h" "VulkanBase/Implementations/Initialization/PipelineInit.cpp" "VulkanBase/Implementations/Initialization/CommandBufferInit.cpp" "VulkanBase/Implementations/RuntimeFunctions.cpp" "VulkanBase/Implementations/Initialization/SemaphoresFencesInit.cpp")

set(OBJ_FILE "${CMAKE_SOURCE_DIR}/Project/Resources/lowpoly_bunny.obj")
add_custom_target(BunnyObj SOURCES ${OBJ_FILE})

# Create the execut
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES}  "Abstraction/Mesh.h" "Abstraction/Mesh.cpp" "Abstraction/Camera.h" "Abstraction/Utils.h")
add_dependencies(${PROJECT_NAME} Shaders BunnyObj)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)


configure_file(${CMAKE_SOURCE_DIR}/Project/Resources/lowpoly_bunny.obj ${CMAKE_CURRENT_BINARY_DIR}/Resources/lowpoly_bunny.obj COPYONLY)