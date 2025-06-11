#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
//#include <curl/curl.h>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

#include "Enemy/BadEnemy.hpp"
#include "Enemy/BinaryEnemy.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/NewEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/Obstacle.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "Generator/ProceduralMapGenerator.hpp"
#include "PlayScene.hpp"
#include "Turret/AntiAirTurret.hpp"
#include "Turret/FreezeTurret.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/Turret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/FireTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "Turret/CoinGen.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/Upgrade_system.hpp"


UpgradeSystem* upgradeSystem;
#include "allegro5/allegro_primitives.h"
#include "allegro5/color.h"

bool PlayScene::shovelActive = false;

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = {
    Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0),
    Engine::Point(0, 1)};
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint =
    Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    /*ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER*/
};

Engine::Point PlayScene::GetClientSize()
{
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize()
{
    //mapTurret.clear();
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(ObstacleGroup = new Group());



    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    AddNewControlObject(ButtonsGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance =
        Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
    shovelActive = false;

    paused = false;
    PrevSpeedMult = 1;
}

void PlayScene::Terminate()
{
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void PlayScene::Update(float deltaTime)
{
    // If we use deltaTime directly, then we might have Bullet-through-paper
    // problem. Reference: Bullet-Through-Paper
    if (!paused)
    {
        if (SpeedMult == 0)
            deathCountDown = -1;
        else if (deathCountDown != -1)
            SpeedMult = 1;
    }
    // Calculate danger zone.

    std::vector<float> reachEndTimes;
    for (auto &it : EnemyGroup->GetObjects()) {
        reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many
    // enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto &it : reachEndTimes) {
        if (it <= DangerTime) {
            danger--;
            if (danger <= 0) {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown) {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample(
                            "astronomia.ogg", false, AudioHelper::BGMVolume,
                            pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(
                    0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0) {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;

    for (int i = 0; i < SpeedMult; i++) {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) {
            if (EnemyGroup->GetObjects().empty()) {
                // Free resources.
                /*delete TileMapGroup;
                delete GroundEffectGroup;
                delete DebugIndicatorGroup;
                delete TowerGroup;
                delete EnemyGroup;
                delete BulletGroup;
                delete EffectGroup;
                delete UIGroup;
                delete imgTarget;*/
                // Win.
                Engine::GameEngine::GetInstance().ChangeScene("win");
                return;
            }
            continue;
        }
        auto current = enemyWaveData.front();
        if (ticks < current.second)
            continue;
        ticks -= current.second;
        enemyWaveData.pop_front();
        const Engine::Point SpawnCoordinate =
            Engine::Point(SpawnGridPoint.x * BlockSize + (double)BlockSize / 2,
                          SpawnGridPoint.y * BlockSize + (double)BlockSize / 2);
        Enemy *enemy;
        switch (current.first) {
        case 1:
            EnemyGroup->AddNewObject(
                enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 2:
            EnemyGroup->AddNewObject(
                enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;

        case 3:
            EnemyGroup->AddNewObject(
                enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 4:
            EnemyGroup->AddNewObject(
                enemy = new NewEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 5:
            EnemyGroup->AddNewObject(
                enemy = new BinaryEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        case 6:
            EnemyGroup->AddNewObject(
                enemy = new BadEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
            break;
        default:
            continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview && !paused) {
        preview->Position =
            Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); ) {
        it->timer -= deltaTime;
        if (it->timer <= 0)
            it = floatingTexts.erase(it);
        else
            ++it;
    }

}

void PlayScene::Draw() const
{
    IScene::Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for
                    // debugging.
                    Engine::Label label(
                        std::to_string(mapDistance[i][j]), "romulus.ttf", 32,
                        (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
    if (paused)
    {
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
        al_draw_filled_rectangle(0, 0, 1280, h,
                                al_map_rgba(0, 0, 0, 100));

        Engine::Label pauseLabel("PAUSED", "romulus.ttf", 256,
                                (double)w / 2.5, (double)h / 2);
        pauseLabel.Color = al_map_rgba(255, 255, 255, 255);
        pauseLabel.Anchor = Engine::Point(0.5, 0.5);
        pauseLabel.Draw();
    }
    for (auto& ft : floatingTexts) {
        al_draw_text(Engine::Resources::GetInstance().GetFont("romulus.ttf", 52).get(),
                      al_map_rgb(0, 0, 0),
                      ft.position.x,
                      ft.position.y - (1.0f - ft.timer / 1.0f) * 30, // 上浮動畫
                      ALLEGRO_ALIGN_CENTER, ft.text.c_str());
    }

}

void PlayScene::OnMouseDown(int button, int mx, int my)
{
    if (!paused)
    {
        if ((button & 1) && !imgTarget->Visible && preview) {
            // Cancel turret construct.
            UIGroup->RemoveObject(preview->GetObjectIterator());
            preview = nullptr;
        }
        if (shovelActive) {
            // remove the sprite
            UIGroup->RemoveObject(shovel->GetObjectIterator());
            shovelActive = false;
            const int x = mx / BlockSize;
            const int y = my / BlockSize;
            if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
                return;
            // check if occupied
            if (mapState[y][x] == TILE_OCCUPIED) {
                mapState[y][x] = TILE_FLOOR;
                // remove the turret
                for (auto &it : TowerGroup->GetObjects()) {
                    if (it->Position.x == x * BlockSize + (double)BlockSize / 2 &&
                        it->Position.y == y * BlockSize + (double)BlockSize / 2) {
                        EarnMoney(dynamic_cast<Turret *>(it)->GetPrice() / 2);
                        TowerGroup->RemoveObject(it->GetObjectIterator());
                        // create sfx of shovel
                        // TODO: add to credits.md
                        AudioHelper::PlayAudio("shovel.ogg");
                        break;
                    }
                }
            }
        }
    }
    if (button == 2) { // 右鍵
        for (auto& obj : UIGroup->GetObjects()) {
                    TurretButton* btn = dynamic_cast<TurretButton*>(obj);
                    if (!btn) continue;
                    Engine::Point diff = Engine::Point(mx, my) - Engine::Point(btn->Position.x, btn->Position.y);

        }

        for (auto& obj : TowerGroup->GetObjects()) {
            Turret* turret = dynamic_cast<Turret*>(obj);
            if (!turret) continue;
            Engine::Point diff = turret->Position - Engine::Point(mx, my);
            if (diff.Magnitude() <= 20) {
                int nextLevel = turret->GetLevel() + 1;
                if (nextLevel <= 5) {
                    turret->Upgrade(nextLevel);

                    // 🔥 加入升級提示
                    floatingTexts.push_back({
                        turret->Position,
                        "Level Up!",
                        1.0f // 1秒消失
                    });

                    //AudioHelper::PlayAudio("upgrade.wav");
                }
                return; // 點到就升級完，結束
            }
        }
    }

    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    if (shovelActive) {
        // remove the sprite
        UIGroup->RemoveObject(shovel->GetObjectIterator());
        shovelActive = false;
        const int x = mx / BlockSize;
        const int y = my / BlockSize;
        if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
            return;
        // check if occupied
        if (mapState[y][x] == TILE_OCCUPIED) {
            mapState[y][x] = TILE_FLOOR;
            // remove the turret
            for (auto &it : TowerGroup->GetObjects()) {
                if (it->Position.x == x * BlockSize + (double)BlockSize / 2 &&
                    it->Position.y == y * BlockSize + (double)BlockSize / 2) {
                    EarnMoney(dynamic_cast<Turret *>(it)->GetPrice() / 2);
                    TowerGroup->RemoveObject(it->GetObjectIterator());
                    // create sfx of shovel
                    // TODO: add to credits.md
                    AudioHelper::PlayAudio("shovel.ogg");
                    break;
                }
            }
        }
    }

    IScene::OnMouseDown(button, mx, my);
}

void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
    if (!paused)
    {
        const int x = mx / BlockSize;
        const int y = my / BlockSize;

        if (shovelActive) {
            shovel->Position.x = mx;
            shovel->Position.y = my;
        }

        if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
            imgTarget->Visible = false;
            return;
        }
        imgTarget->Visible = true;
        imgTarget->Position.x = x * BlockSize;
        imgTarget->Position.y = y * BlockSize;
    }
}

void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);

    if (!paused)
    {
        if (!imgTarget->Visible)
            return;
        const int x = mx / BlockSize;
        const int y = my / BlockSize;
        if (button & 1) {
            if (mapState[y][x] != TILE_OCCUPIED) {
                if (!preview)
                    return;
                // Check if valid.
                if (!CheckSpaceValid(x, y)) {
                    Engine::Sprite *sprite;
                    GroundEffectGroup->AddNewObject(
                        sprite =
                            new DirtyEffect("play/target-invalid.png", 1,
                                            x * BlockSize + (double)BlockSize / 2,
                                            y * BlockSize + (double)BlockSize / 2));
                    sprite->Rotation = 0;
                    return;
                }
                // Purchase.
                EarnMoney(-preview->GetPrice());
                // Remove Preview.
                preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview->GetObjectIterator());
                // Construct real turret.
                preview->Position.x = x * BlockSize + (double)BlockSize / 2;
                preview->Position.y = y * BlockSize + (double)BlockSize / 2;
                preview->Enabled = true;
                preview->Preview = false;
                preview->Tint = al_map_rgba(255, 255, 255, 255);
                TowerGroup->AddNewObject(preview);
                // To keep responding when paused.
                preview->Update(0);

                preview->SetJustPlaced();//給進化砲塔用的，放置瞬間有效果，不要刪掉
                // Remove Preview.
                preview = nullptr;
            }
        }
    }
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        if (mapState[y][x] != TILE_OCCUPIED) {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y)) {
                Engine::Sprite *sprite;
                GroundEffectGroup->AddNewObject(
                    sprite = new DirtyEffect("play/target-invalid.png", 1,
                                             x * BlockSize + (double)BlockSize / 2,
                                             y * BlockSize + (double)BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + (double)BlockSize / 2;
            preview->Position.y = y * BlockSize + (double)BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);


            if(preview->level == 6){
                preview->SetJustPlaced();
            }

            // Remove Preview.
            preview = nullptr;

            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}

void PlayScene::OnKeyDown(int keyCode)
{
    IScene::OnKeyDown(keyCode);
    if (!paused)
    {
        if (keyCode == ALLEGRO_KEY_TAB) {
            DebugMode = !DebugMode;
        }
        else {
            keyStrokes.push_back(keyCode);
            if (keyStrokes.size() > code.size())
                keyStrokes.pop_front();
            if (keyStrokes.size() == code.size()) {
                bool match = true;
                auto stroke = keyStrokes.begin();
                for (auto i : code) {
                    if (stroke == keyStrokes.end() || *stroke != i) {
                        match = false;
                        break;
                    }
                    ++stroke;
                }
                if (match) {
                    EffectGroup->AddNewObject(new Plane());
                    EarnMoney(10000);
                    keyStrokes.clear();
                }
            }
        }
        if (keyCode == ALLEGRO_KEY_S) {
            UIBtnClicked(0);
        }
        if (keyCode == ALLEGRO_KEY_Q) {
            // Hotkey for MachineGunTurret.
            UIBtnClicked(1);
        }
        else if (keyCode == ALLEGRO_KEY_W) {
            // Hotkey for LaserTurret.
            UIBtnClicked(2);
        }
        else if (keyCode == ALLEGRO_KEY_E) {
            // Hotkey for FreezeTurret
            UIBtnClicked(3);
        }
        else if (keyCode == ALLEGRO_KEY_R) {
            // Hotkey for FreezeTurret
            UIBtnClicked(4);
        }
        else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
            // Hotkey for Speed up.
            SpeedMult = keyCode - ALLEGRO_KEY_0;
        }
    }
    
    keyStrokes.push_back(keyCode);
    if (keyStrokes.size() > code.size())
        keyStrokes.pop_front();
    if (keyStrokes.size() == code.size()) {
        bool match = true;
        auto stroke = keyStrokes.begin();
        for (auto i : code) {
            if (stroke == keyStrokes.end() || *stroke != i) {
                match = false;
                break;
            }
            ++stroke;
        }
        if (match) {
            EffectGroup->AddNewObject(new Plane());
            EarnMoney(10000);
            keyStrokes.clear();
        }
    }
}

void PlayScene::Hit()
{
    lives--;
    UILives->Text = std::string("LIFE ") + std::to_string(lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}

int PlayScene::GetMoney() const { return money; }

void PlayScene::EarnMoney(int money)
{
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}

void PlayScene::ReadMap()
{
    // procedural map generation code
    if (MapId == 4) {
        auto seed = std::random_device{}();
        ProceduralMapGenerator generator(seed);
        auto proceduralMap = generator.generateMap(3);

        // Prepare mapState
        mapState = std::vector<std::vector<TileType>>(
            MapHeight, std::vector<TileType>(MapWidth));
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                mapState[i][j] = proceduralMap[i][j] ? TILE_FLOOR : TILE_DIRT;
            }
        }
    }
    else {
        // Load map from file
        std::string filename = "Resource/map" + std::to_string(MapId) + ".txt";
        char c;
        std::vector<bool> mapData;
        std::ifstream fin(filename);
        while (fin >> c) {
            switch (c) {
            case '0':
                mapData.push_back(false);
                break;
            case '1':
                mapData.push_back(true);
                break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted.");
                break;
            default:
                throw std::ios_base::failure("Map data is corrupted.");
            }
        }
        fin.close();

        if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
            throw std::ios_base::failure("Map data is corrupted.");

        // Convert to mapState
        mapState = std::vector<std::vector<TileType>>(
            MapHeight, std::vector<TileType>(MapWidth));
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                const int num = mapData[i * MapWidth + j];
                mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            }
        }
    }

    // Second pass: Apply autotiling
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            if (mapState[i][j] == TILE_FLOOR) {
                TileMapGroup->AddNewObject(
                    new Engine::Image("play/grass_floor.png", j * BlockSize,
                                      i * BlockSize, BlockSize, BlockSize));
            }
            else {
                int mask = 0;
                if (i > 0 && mapState[i - 1][j] == TILE_DIRT)
                    mask |= 1; // N
                if (j < MapWidth - 1 && mapState[i][j + 1] == TILE_DIRT)
                    mask |= 2; // E
                if (i < MapHeight - 1 && mapState[i + 1][j] == TILE_DIRT)
                    mask |= 4; // S
                if (j > 0 && mapState[i][j - 1] == TILE_DIRT)
                    mask |= 8; // W
                if (i == 0 && j == 0)
                    mask |= 8;
                if (i == MapHeight - 1 && j == MapWidth - 1)
                    mask |= 2;

                std::string filename =
                    "play/dirt" + std::to_string(mask + 1) + ".png";
                TileMapGroup->AddNewObject(
                    new Engine::Image(filename, j * BlockSize, i * BlockSize,
                                      BlockSize, BlockSize));
            }
        }
    }

    //generate rocks
    for(int r=0; r<6; r++){
        int x = rand() % MapWidth;
        int y = rand() % MapHeight;
        if(mapState[y][x] == TILE_FLOOR){
            Obstacle* obs = new Obstacle("play/rock.png",
                                         x * BlockSize + BlockSize / 2,
                                         y * BlockSize + BlockSize / 2,
                                         100, x, y); // 把格子座標帶進去

            ObstacleGroup->AddNewObject(obs);
            mapState[y][x] = TILE_OCCUPIED;
        }


    }

}

void PlayScene::ReadEnemyWave()
{
    std::string filename =
        std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI()
{

    if (superEvolutionEnabled) {
        UIGroup->AddNewObject(new Engine::Label(
            "Next turret: SUPER EVOLUTION!",
            "romulus.ttf", 52, 1300, 300, 255, 0, 0, 255
        ));
    }

    // Background
    UIGroup->AddNewObject(
        new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(
        new Engine::Label(std::string("Stage ") + std::to_string(MapId),
                          "romulus.ttf", 64, 1280 + 25, 0));
    UIGroup->AddNewObject(
        UIMoney = new Engine::Label(std::string("$") + std::to_string(money),
                                    "romulus.ttf", 48, 1280 + 25, 50));
    UIGroup->AddNewObject(UILives = new Engine::Label(
                              std::string("Life ") + std::to_string(lives),
                              "romulus.ttf", 48, 1280 + 25, 85));
    TurretButton *btn;
    // Button 1
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/shovel-base.png", 1500, 92 - 36, 64, 64, 0, 0),
        Engine::Sprite("play/shovel.png", 1500, 92 - 36, 64, 64, 0, 0), 1500,
        92 - 36, 0);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);

    // Button 1
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1294, 150, 0, 0, 0, 0),
        Engine::Sprite("play/turret-1.png", 1294, 150 - 8, 0, 0, 0, 0), 1294,
        150, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1370, 150, 0, 0, 0, 0),
        Engine::Sprite("play/turret-2.png", 1370, 150 - 8, 0, 0, 0, 0), 1370,
        150, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    // Button 3
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1446, 150, 0, 0, 0, 0),
        Engine::Sprite("play/turret-5.png", 1446, 150 - 8, 0, 0, 0, 0), 1446,
        150, AntiAirTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);

    //freezeTurret
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1294, 226, 0, 0, 0, 0), //x+76 y+76
        Engine::Sprite("play/turret-6.png", 1294, 226 - 8, 0, 0, 0, 0), 1294,
        226, FreezeTurret::Price);

    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);

    //fireTurret
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1370, 226, 0, 0, 0, 0), //x+76 y+76
        Engine::Sprite("play/turret-6.png", 1370, 226 - 8, 0, 0, 0, 0), 1370,
        226, FreezeTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 6));
    UIGroup->AddNewControlObject(btn);

    //CoinGen
    btn = new TurretButton(
            "play/floor.png", "play/dirt.png",
            Engine::Sprite("play/tower-base.png", 1446, 226, 0, 0, 0, 0), //x+76 y+76
            Engine::Sprite("play/turret-6.png", 1446, 226 - 8, 0, 0, 0, 0), 1446,
            226, FreezeTurret::Price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 7));
        UIGroup->AddNewControlObject(btn);

    // 進化按鈕
    btn = new TurretButton(
            "play/floor.png", "play/dirt.png",
            Engine::Sprite("play/tower-base.png", 1446, 440, 0, 0, 0, 0),
            Engine::Sprite("play/turret-7.png", 1446, 440 - 8, 0, 0, 0, 0), 1446,
            440, 0); // 進化按鈕不需要金錢檢查
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 5));
        UIGroup->AddNewControlObject(btn);

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;

    dangerIndicator =
        new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);

    double halfW = (double)w / 2;
    double halfH = (double)h / 2;

    Engine::ImageButton *btn1;
    btn1 = new Engine::ImageButton("clickable/pause_normal.png",
                                   "clickable/pause_hover.png", w - 212,
                                   (double)halfH * 1.55 - 50, 100, 100);
    btn1->SetOnClickCallback(std::bind(&PlayScene::PauseOrResume, this));
    UIGroup->AddNewControlObject(btn1);
    btn1 = new Engine::ImageButton("clickable/restart_normal.png",
                                   "clickable/restart_hover.png", w - 274,
                                   (double)halfH * 1.8 - 50, 100, 100);
    btn1->SetOnClickCallback(std::bind(&PlayScene::RestartOnClick, this));
    UIGroup->AddNewControlObject(btn1);
    btn1 = new Engine::ImageButton("clickable/quit_normal.png",
                                   "clickable/quit_hover.png", w - 156,
                                   (double)halfH * 1.8 - 50, 100, 100);
    btn1->SetOnClickCallback(std::bind(&PlayScene::QuitOnClick, this));
    UIGroup->AddNewControlObject(btn1);
}

void PlayScene::UIBtnClicked(int id)
{
    if (paused) return;
    Turret *next_preview = nullptr;
    if (id == 1 && money >= MachineGunTurret::Price)
        next_preview = new MachineGunTurret(0, 0);
    else if (id == 2 && money >= LaserTurret::Price)
        next_preview = new LaserTurret(0, 0);
    else if (id == 3 && money >= AntiAirTurret::Price)
        next_preview = new AntiAirTurret(0, 0);
    else if (id == 4 && money >= FreezeTurret::Price)
        next_preview = new FreezeTurret(0, 0);
    else if (id == 5) {
                 superEvolutionEnabled = true;
                 //AudioHelper::PlayAudio("upgrade.wav"); // 提示音效
                 // 也可以加入浮動提示
                 floatingTexts.push_back({
                     Engine::Point(1300, 200),
                     "Super Evolution Ready!",
                     1.0f
                 });
                 return;
    }
    else if (id == 6 && money >= FireTurret::Price)
            next_preview = new FireTurret(0, 0);
    else if (id == 7 && money >= CoinGen::Price)
            next_preview = new CoinGen(0, 0);
    else if (id == 0) {
        ALLEGRO_MOUSE_STATE mouse_state;
        al_get_mouse_state(&mouse_state);
        // Shovel
        if (shovelActive) {
            UIGroup->RemoveObject(shovel->GetObjectIterator());
            shovelActive = false;
            return;
        }
        shovelActive = true;
        shovel =
            new Engine::Sprite("play/shovel.png", mouse_state.x, mouse_state.y);
        shovel->Tint = al_map_rgba(255, 255, 255, 200);
        UIGroup->AddNewObject(shovel);
        return;
    }

    if (!next_preview)
        return; // not enough money or invalid turret.

    if (superEvolutionEnabled) {
            next_preview->Upgrade(6);
            superEvolutionEnabled = false; // 一次性
            floatingTexts.push_back({
                Engine::Point(1300, 250),
                "Super Evolution Applied!",
                1.0f
            });
        }

    if (preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = next_preview;
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview->Tint = al_map_rgba(255, 255, 255, 200);
        preview->Enabled = false;
        preview->Preview = true;
        UIGroup->AddNewObject(preview);
        OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x,
                    Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y)
{
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto &it : EnemyGroup->GetObjects()) {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0)
            pnt.x = 0;
        if (pnt.x >= MapWidth)
            pnt.x = MapWidth - 1;
        if (pnt.y < 0)
            pnt.y = 0;
        if (pnt.y >= MapHeight)
            pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto &it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance()
{
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(
        MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        for (auto dir : directions) {
            int nx = p.x + dir.x;
            int ny = p.y + dir.y;

            if (nx >= 0 && nx < MapWidth && ny >= 0 && ny < MapHeight &&
                map[ny][nx] == -1 && (mapState[ny][nx] == TILE_DIRT)) {
                map[ny][nx] = map[p.y][p.x] + 1;
                que.push(Engine::Point(nx, ny));
            }
        }
    }
    return map;
}

void PlayScene::PauseOrResume(void)
{
    if (!paused)
    {
        PrevSpeedMult = SpeedMult;
        SpeedMult = 0;
        paused = true;
    }
    else {
        SpeedMult = PrevSpeedMult;
        PrevSpeedMult = 1;
        paused = false;
    }
}

void PlayScene::QuitOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void PlayScene::RestartOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void PlayScene::TriggerCheatCode()
{
    EffectGroup->AddNewObject(new Plane());
    EarnMoney(10000);
    keyStrokes.clear();
}
