#include "VulkanBase/VulkanBase.h"

void VulkanBase::InitializeRayTracing()
{
	VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	properties2.pNext = &m_RTProperties;
	vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &properties2);

	m_RTBuilder.setup(m_Device, &m_alloc, 0);
}
void VulkanBase::DestroyRayTracing()
{
	m_RTBuilder.destroy();
}

void VulkanBase::CreateTLAS()
{
	std::vector<VkAccelerationStructureInstanceKHR> tlas;
	tlas.reserve(m_Scene->GetMeshesAmount());
	for (const auto& mesh : m_Scene->GetMeshes())
	{
		VkAccelerationStructureInstanceKHR rayInst{};
		rayInst.transform = nvvk::toTransformMatrixKHR(mesh->GetModelMatrix());  // Position of the instance
		rayInst.instanceCustomIndex = mesh->GetMeshIndex();                               // gl_InstanceCustomIndexEXT
		rayInst.accelerationStructureReference = m_RTBuilder.getBlasDeviceAddress(mesh->GetMeshIndex());
		rayInst.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		rayInst.mask = 0xFF;       //  Only be hit if rayMask & instance.mask != 0
		rayInst.instanceShaderBindingTableRecordOffset = 0;  // We will use the same hit group for all objects
		tlas.emplace_back(rayInst);
	}
	m_RTBuilder.buildTlas(tlas, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);

}

auto VulkanBase::ObjectToVkGeometryKHR(Mesh* mesh)
{
	// Create the Vertex and Index addresses
	VkBufferDeviceAddressInfo vertexAddressInfo{};
	vertexAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	vertexAddressInfo.buffer = m_VertexBuffers[mesh->GetMeshIndex()];

	VkBufferDeviceAddressInfo indexAddressInfo{};
	indexAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	indexAddressInfo.buffer = m_IndexBuffers[mesh->GetMeshIndex()];

	VkDeviceAddress vertexAddress{vkGetBufferDeviceAddress(m_Device, &vertexAddressInfo)};
	VkDeviceAddress indexAddress{ vkGetBufferDeviceAddress(m_Device, &indexAddressInfo)};


	// Triangle Information
	VkAccelerationStructureGeometryTrianglesDataKHR triangles{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR };
	triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;  // vec3 vertex position data.
	triangles.vertexData.deviceAddress = vertexAddress;
	triangles.vertexStride = sizeof(Vertex);
	triangles.indexType = VK_INDEX_TYPE_UINT32;
	triangles.indexData.deviceAddress = indexAddress;
	// Indicate identity transform by setting transformData to null device pointer.
	triangles.transformData = {};
	triangles.maxVertex = uint32_t(mesh->GetVertices().size() - 1);


	// Identify the above data as containing opaque triangles.
	VkAccelerationStructureGeometryKHR asGeom{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR };
	asGeom.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	asGeom.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	asGeom.geometry.triangles = triangles;

	// The entire array will be used to build the BLAS.
	VkAccelerationStructureBuildRangeInfoKHR offset;
	offset.firstVertex = 0;
	offset.primitiveCount = uint32_t(mesh->GetIndices().size() / 3);
	offset.primitiveOffset = 0;
	offset.transformOffset = 0;

	// Our blas is made from only one geometry, but could be made of many geometries
	nvvk::RaytracingBuilderKHR::BlasInput input;
	input.asGeometry.emplace_back(asGeom);
	input.asBuildOffsetInfo.emplace_back(offset);

	return input;
}
void VulkanBase::CreateBLASes()
{
	std::vector<nvvk::RaytracingBuilderKHR::BlasInput> allBlas{};

	allBlas.resize(m_Scene->GetMeshesAmount());

	for (const auto& mesh : m_Scene->GetMeshes())
	{
		nvvk::RaytracingBuilderKHR::BlasInput blas = ObjectToVkGeometryKHR(mesh);

		// We could add more geometry in each BLAS, but we add only one for now
		allBlas.emplace_back(blas);
	}

	m_RTBuilder.buildBlas(allBlas, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
}