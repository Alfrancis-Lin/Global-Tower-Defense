#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "Enemy/BinaryEnemy.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/NewEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
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
#include "Turret/MachineGunTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"

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
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
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
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
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
            Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2,
                          SpawnGridPoint.y * BlockSize + BlockSize / 2);
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
        default:
            continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview) {
        preview->Position =
            Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
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
}

void PlayScene::OnMouseDown(int button, int mx, int my)
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

    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my)
{
    IScene::OnMouseMove(mx, my);
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
void PlayScene::OnMouseUp(int button, int mx, int my)
{
    IScene::OnMouseUp(button, mx, my);
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
    if (MapId == 3) {
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
                if (i == 0 && j == 0) mask |= 8;
                if (i == MapHeight - 1 && j == MapWidth - 1) mask |= 2;

                std::string filename =
                    "play/dirt" + std::to_string(mask + 1) + ".png";
                TileMapGroup->AddNewObject(
                    new Engine::Image(filename, j * BlockSize, i * BlockSize,
                                      BlockSize, BlockSize));
            }
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
    // Background
    UIGroup->AddNewObject(
        new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(
        new Engine::Label(std::string("Stage ") + std::to_string(MapId),
                          "romulus.ttf", 32, 1294, 0));
    UIGroup->AddNewObject(
        UIMoney = new Engine::Label(std::string("$") + std::to_string(money),
                                    "romulus.ttf", 24, 1294, 48));
    UIGroup->AddNewObject(UILives = new Engine::Label(
                              std::string("Life ") + std::to_string(lives),
                              "romulus.ttf", 24, 1294, 88));
    TurretButton *btn;
    // Button 1
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/shovel-base.png", 1500, 60, 0, 0, 0, 0),
        Engine::Sprite("play/shovel.png", 1500, 60 - 8, 0, 0, 0, 0), 1500, 60,
        0);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    // Button 1
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1294, 136, 0, 0, 0, 0),
        Engine::Sprite("play/turret-1.png", 1294, 136 - 8, 0, 0, 0, 0), 1294,
        136, MachineGunTurret::Price);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    // Button 2
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1370, 136, 0, 0, 0, 0),
        Engine::Sprite("play/turret-2.png", 1370, 136 - 8, 0, 0, 0, 0), 1370,
        136, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    // Button 3
    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1446, 136, 0, 0, 0, 0),
        Engine::Sprite("play/turret-5.png", 1446, 136 - 8, 0, 0, 0, 0), 1446,
        136, AntiAirTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);

    btn = new TurretButton(
        "play/floor.png", "play/dirt.png",
        Engine::Sprite("play/tower-base.png", 1294, 212, 0, 0, 0, 0),
        Engine::Sprite("play/turret-6.png", 1294, 212 - 8, 0, 0, 0, 0), 1294,
        212, FreezeTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 4));
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
    btn1 = new Engine::ImageButton("clickable/dirt.png",
                                   "clickable/floor.png", halfW * 1.8,
                                   (double)halfH * 1.8 - 50, 100, 100);
    btn1->SetOnClickCallback(std::bind(&PlayScene::BackOnClick, this, 0));
    AddNewControlObject(btn1);
    AddNewObject(new Engine::Label("HOME", "romulus.ttf", 20, halfW * 1.8 + 50,
                                   (double)halfH * 1.8, 0, 0, 0, 255, 0.5,
                                   0.5));
}

void PlayScene::UIBtnClicked(int id)
{
    Turret *next_preview = nullptr;
    if (id == 1 && money >= MachineGunTurret::Price)
        next_preview = new MachineGunTurret(0, 0);
    else if (id == 2 && money >= LaserTurret::Price)
        next_preview = new LaserTurret(0, 0);
    else if (id == 3 && money >= AntiAirTurret::Price)
        next_preview = new AntiAirTurret(0, 0);
    else if (id == 4 && money >= FreezeTurret::Price)
        next_preview = new FreezeTurret(0, 0);
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

        // TODO PROJECT-1 (1/1): Implement a BFS starting from the most
        // right-bottom block in the map.
        //               For each step you should assign the corresponding
        //               distance to the most right-bottom block. mapState[y][x]
        //               is TILE_DIRT if it is empty.
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

void PlayScene::BackOnClick(int stage)
{
    if (stage == 0) {
        Engine::GameEngine::GetInstance().ChangeScene("start");
    }
}

void PlayScene::TriggerCheatCode() {
    EffectGroup->AddNewObject(new Plane());
    EarnMoney(10000);
    keyStrokes.clear();
}
