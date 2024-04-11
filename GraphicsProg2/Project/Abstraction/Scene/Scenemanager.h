#pragma once

#include <vector>
#include <memory>

#include "../Singleton.h"
#include "Scene.h"

class SceneManager : public Singleton<SceneManager>
{
public:

    Scene* CreateScene()
    {
        m_Scenes.emplace_back(std::make_unique<Scene>());
        ++m_CurrentSceneIndex;
        return GetCurrentScene();
    }
    Scene* GetCurrentScene() { return m_Scenes[m_CurrentSceneIndex].get(); }

private:

    std::vector<std::unique_ptr<Scene>> m_Scenes{};
    int m_CurrentSceneIndex{ -1 };

};