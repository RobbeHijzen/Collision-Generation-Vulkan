#include "MachineShader.h"

MachineShader::MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	: m_VertexShaderFile{vertexShaderFile}
	, m_FragmentShaderFile{fragmentShaderFile}
{
}
