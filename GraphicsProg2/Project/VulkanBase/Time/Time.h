#pragma once

#include "Vulkanbase/Scene/Singleton.h"
#include <chrono>

using namespace std::chrono;


struct Time final : public Singleton<Time>
{
public:

	void Update();

	float GetDeltaTime() const { return m_DeltaTime; }
	float GetFps() const { return m_Fps; }

private:

	float m_DeltaTime{};
	float m_Fps{};

	const float m_MaxDeltaTime{ 1.f };

	steady_clock::time_point m_LastTime{ high_resolution_clock::now() };
	steady_clock::time_point m_CurrentTime{};
};
