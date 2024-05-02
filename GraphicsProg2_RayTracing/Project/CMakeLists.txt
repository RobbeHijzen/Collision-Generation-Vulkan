# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders")

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

    "Abstraction/Shaders/DerivedShaders/Shader3D.h"
    "Abstraction/Shaders/DerivedShaders/Shader3D.cpp" 
    "Abstraction/Scene/Scene.h" 
    "Abstraction/Scene/Scene.cpp" 
    "Abstraction/Singleton.h" 
    "Abstraction/Mesh.h" 
    "Abstraction/Mesh.cpp" 
    "Abstraction/Camera.h" 
    "Abstraction/Utils.h"
     
    "Abstraction/VertexInfo.h" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/InstanceInit.cpp"
    "VulkanBase/Implementations/Initialization/BaseVulkan/PhysicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/LogicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/SwapChainInit.cpp"
    "VulkanBase/Implementations/Initialization/BaseVulkan/PipelineInit.cpp" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/CommandBufferInit.cpp" 
    "VulkanBase/Implementations/RuntimeFunctions.cpp" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/SemaphoresFencesInit.cpp" 
    "VulkanBase/Implementations/Initialization/BaseVulkan/VertexBufferInit.cpp" 
    "VulkanBase/Implementations/BufferHelpers.cpp"
    "VulkanBase/Implementations/Initialization/BaseVulkan/DescriptorInit.cpp"
    "VulkanUtil/stb_image.h" "VulkanBase/Implementations/Initialization/BaseVulkan/TextureInit.cpp"
    "VulkanBase/Implementations/Initialization/BaseVulkan/DepthBufferingInit.cpp" 
    "Abstraction/HelperStructs.h"
    "VulkanBase/Implementations/Initialization/RayTracing/RayTracingSetup.cpp"
    )
    

# Copies the Resources folder into out
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/Project/Resources")
file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(CopyResources ALL
    COMMENT "Copying resources directory to build directory"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCES_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${RESOURCES_DIR}
)

#add_subdirectory(nvpro_core)


# Create the execut
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} ${RayTracing_SRC})
add_dependencies(${PROJECT_NAME} Shaders)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)

