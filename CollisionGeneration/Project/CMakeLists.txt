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

    "VulkanBase/Shaders/DerivedShaders/Shader3D.h"
    "VulkanBase/Shaders/DerivedShaders/Shader3D.cpp" 
    "vulkanbase/Scene/Scene.h" 
     
    "VulkanBase/Scene/Singleton.h" 
     
    "VulkanBase/HelperStructs/VertexInfo.h" 
    "VulkanBase/Implementations/Initialization/InstanceInit.cpp"
    "VulkanBase/Implementations/Initialization/PhysicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/LogicalDeviceInit.cpp" 
    "VulkanBase/Implementations/Initialization/SwapChainInit.cpp"
    "VulkanBase/Implementations/Initialization/PipelineInit.cpp" 
    "VulkanBase/Implementations/Initialization/CommandBufferInit.cpp" 
    "VulkanBase/Implementations/RuntimeFunctions.cpp" 
    "VulkanBase/Implementations/Initialization/SemaphoresFencesInit.cpp" 
    "VulkanBase/Implementations/Initialization/VertexBufferInit.cpp" 
    "VulkanBase/Implementations/BufferHelpers.cpp"
    "VulkanBase/Implementations/Initialization/DescriptorInit.cpp"
    "VulkanUtil/stb_image.h" "VulkanBase/Implementations/Initialization/TextureInit.cpp"
    "VulkanBase/Implementations/Initialization/DepthBufferingInit.cpp" 
    
     
    "VulkanBase/HelperStructs/HelperStructs.h" 
    "PhysicsGame/Collision/CollisionFixer.cpp" 
    "VulkanBase/HelperStructs/BaseComponent.h" 
    "PhysicsGame/Components/DerivedComponents/MovementComponent.cpp" 
    "PhysicsGame/Components/DerivedComponents/CollisionComponent.cpp" 
    "PhysicsGame/Components/DerivedComponents/CameraComponent.cpp" 
    "Vulkanbase/EventSystem/Events.h" 
    "Vulkanbase/EventSystem/Observer.h" 
    "PhysicsGame/MathHelpers.h" 
    "vulkanbase/VulkanLoad.cpp" "Vulkanbase/Time/Time.h" "Vulkanbase/Time/Time.cpp" "PhysicsGame/Collision/AABBCalculator.h" "PhysicsGame/Collision/AABBCalculator.cpp" "VulkanBase/HelperStructs/PipelinesEnum.h" "VulkanBase/HelperStructs/IRenderable.h" "VulkanBase/Scene/Object.h"  )
    

# Copies the Resources folder into out
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/Project/Resources")
file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(CopyResources ALL
    COMMENT "Copying resources directory to build directory"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCES_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${RESOURCES_DIR}
)

# Create the execut
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES}  "PhysicsGame/Meshes/Mesh.h" "PhysicsGame/Meshes/Mesh.cpp" "PhysicsGame/Camera.h" "PhysicsGame/Utils.h")
add_dependencies(${PROJECT_NAME} Shaders)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)

