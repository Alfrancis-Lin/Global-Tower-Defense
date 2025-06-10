#ifndef UPGRADE_SYSTEM_HPP
#define UPGRADE_SYSTEM_HPP

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Turret.hpp"

class UpgradeSystem : public Engine::IObject {
protected:

public:
    bool active = false;
    Turret* targetTurret = nullptr;
    UpgradeSystem();
    void Activate(Turret* turret);
    void Deactivate();
    void Update(float deltaTime) override;
    void Draw() const override;

    // 新增：專用的點擊事件
    void OnMouseDown(int button, int mx, int my);
};

#endif // UPGRADE_SYSTEM_HPP
