#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Turret/upgrade_system.hpp"
#include "UI/Component/Label.hpp"

struct FloatingText {
    Engine::Point position;
    std::string text;
    float timer; // 倒數秒數
};

#include "UI/Component/ImageButton.hpp"

class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class PlayScene : public Engine::IScene {
private:

    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

protected:
    int lives;
    int money;
    int SpeedMult;
    UpgradeSystem* upgradeSystem = nullptr;
    Turret* selectedTurret = nullptr;

    bool superEvolutionEnabled = false;



public:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    static bool multiendd;
    static bool multiplay;
    static bool DebugMode;
    float annoying_timer;
    static const std::vector<Engine::Point> directions;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    static bool shovelActive;
    int MapId;
    float ticks;
    float deathCountDown;
    std::vector <int> inputkey;

    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *TowerGroup;
    Group *EnemyGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Group *ButtonsGroup;

    Group *ObstacleGroup;

    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    Engine::Sprite *shovel;
    Turret *preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::vector<std::pair<int, float>> enemyOut;
    std::list<int> keyStrokes;
    std::vector<FloatingText> floatingTexts; //文字生成
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    virtual void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y);
    std::vector<std::vector<int>> CalculateBFSDistance();
    // void ModifyReadMapTiles();
    void TriggerCheatCode();
    void ActionOnClick(int action);
    void PauseOrResume(void);
    bool paused;
    int PrevSpeedMult;
    void QuitOnClick(void);
    void RestartOnClick(void);
    Engine::Label* y;
    Engine::Label* u;
    Engine::Label* ii;
    Engine::Label* o;
    std::unordered_map<int, std::string> enemy_id_to_type;
    Engine::Label* random_trivia;
    std::string fact;
    std::vector<Engine::Label*> trivia;
};
#endif   // PLAYSCENE_HPP
