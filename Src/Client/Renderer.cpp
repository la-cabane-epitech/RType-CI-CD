/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.cpp
*/

#include "Client/Ray.hpp"
#include "Client/Renderer.hpp"
#include "Client/ParallaxLayer.hpp"
#include <map>

#include <string>
#include <optional>
#include <vector>
#include <algorithm>

Renderer::Renderer(GameState& gameState) : _gameState(gameState), _actionToRemap(std::nullopt) {
    _textures[0] = LoadTexture("Assets/r-typesheet42.gif");
    _textures[1] = LoadTexture("Assets/attack.png");
    _textures[2] = LoadTexture("Assets/r-typesheet3.gif");
    _textures[3] = LoadTexture("Assets/r-typesheet5.gif");
    _textures[4] = LoadTexture("Assets/r-typesheet1.gif");

    // Background Textures
    _textures[10] = LoadTexture("Assets/blue-back.png");
    _textures[11] = LoadTexture("Assets/blue-stars.png");
    _textures[12] = LoadTexture("Assets/asteroid-1.png");

    float bgScale = (float)GetScreenHeight() / _textures[10].height;
    _parallaxLayers.emplace_back(0.2f, _textures[10], bgScale, 0.0f);
    _parallaxLayers.emplace_back(0.4f, _textures[11], 6.0f, 0.0f);

    if (ENTITY_REGISTRY.empty()) {
        ENTITY_REGISTRY[1] = { 1, false, 1, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
        ENTITY_REGISTRY[2] = { 2, true, 12, 8.0f, 17.0f, 18.0f, 2.0f, 0.0f, 0.0f };
        ENTITY_REGISTRY[3] = { 3, true, 8, 12.0f, 33.0f, 36.0f, 2.5f, 0.0f, 0.0f };
        ENTITY_REGISTRY[4] = { 4, true, 4, 0.0f, 29.0f, 30.0f, 3.0f, 136.0f, 19.0f };
    }
}

Renderer::~Renderer()
{
    if (IsWindowReady()) {
        for (auto const& [key, val] : _textures)
        {
            UnloadTexture(val);
        }
    }
}

void Renderer::draw(const std::map<std::string, int>& keybinds)
{
    float dt = GetFrameTime();
    float scrollSpeed = 150.0f;

    for (auto& layer : _parallaxLayers) {
        layer.update(dt, scrollSpeed);
    }

    ClearBackground(BLACK);

    for (auto& layer : _parallaxLayers) {
        layer.draw();
    }
    DrawText("R-Type", 10, 10, 20, DARKGRAY);

    std::string pingText = "Ping: " + std::to_string(_gameState.rtt) + " ms";
    DrawText(pingText.c_str(), 10, 40, 20, DARKGRAY);

    for (const auto& pair : _gameState.players) {
        Color color = (pair.first == _gameState.myPlayerId) ? BLUE : RED;

        float targetBank = 2.0f; // 2.0 = Neutre (Frame 2)
        if (pair.first == _gameState.myPlayerId) {
            int upKey = (keybinds.count("UP")) ? keybinds.at("UP") : KEY_UP;
            int downKey = (keybinds.count("DOWN")) ? keybinds.at("DOWN") : KEY_DOWN;
            if (IsKeyDown(upKey)) targetBank = 4.0f;
            else if (IsKeyDown(downKey)) targetBank = 0.0f;
        }

        if (_playerBank.find(pair.first) == _playerBank.end()) _playerBank[pair.first] = 2.0f;
        float& currentBank = _playerBank[pair.first];
        float bankSpeed = 15.0f * dt;

        if (currentBank < targetBank) currentBank = std::min(currentBank + bankSpeed, targetBank);
        else if (currentBank > targetBank) currentBank = std::max(currentBank - bankSpeed, targetBank);

        int frameIndex = static_cast<int>(currentBank);
        int spriteIndex = pair.first % 4;
        int textureId = 0;
        float baseY = 0.0f;

        switch (spriteIndex) {
            case 0: textureId = 0; baseY = 0.0f; break;
            case 1: textureId = 0; baseY = 18.0f; break;
            case 2: textureId = 0; baseY = 36.0f; break;
            case 3: textureId = 0; baseY = 53.0f; break;
            default: textureId = 0; baseY = 0.0f; break;
        }

        if (!_textures.count(textureId)) textureId = 0;

        if (_textures.count(textureId) && _textures.at(textureId).id != 0) {
            const Texture2D& texture = _textures.at(textureId);
            Rectangle sourceRec = { frameIndex * 33.0f, baseY, 33.0f, 17.0f };
            float scale = 2.0f;
            Rectangle destRec = { pair.second.x, pair.second.y, sourceRec.width * scale, sourceRec.height * scale };
            Vector2 origin = { destRec.width / 2, destRec.height / 2 };
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }

        if (pair.first == _gameState.myPlayerId && IsKeyDown(KEY_SPACE)) {
            if (_textures.count(4) && _textures.at(4).id != 0) {
                const Texture2D& texture = _textures.at(4);
                int currentFrame = static_cast<int>(GetTime() * 10.0f) % 8;
                Rectangle chargeRec = { 0.0f + currentFrame * 33.0f, 49.0f, 33.0f, 36.0f };
                float chargeScale = 2.0f;
                Rectangle destRec = { 
                    pair.second.x + 10.0f, 
                    pair.second.y - (chargeRec.height * chargeScale) / 2.0f,
                    chargeRec.width * chargeScale, 
                    chargeRec.height * chargeScale 
                };
                DrawTexturePro(texture, chargeRec, destRec, {0, 0}, 0.0f, WHITE);
            }
        }
    }

    for (const auto& pair : _gameState.entities) {
        const auto& entity = pair.second;

        if (ENTITY_REGISTRY.count(entity.type)) {
            const auto& config = ENTITY_REGISTRY[entity.type];

            if (_textures.count(config.textureId)) {
                const Texture2D& texture = _textures.at(config.textureId);
                Rectangle sourceRec;

                if (config.isAnimated) {
                    int currentFrame = static_cast<int>(GetTime() * config.frameSpeed) % config.frameCount;
                    sourceRec = { config.startX + currentFrame * config.width, config.startY, config.width, config.height };
                } else {
                    float w = (config.width > 0) ? config.width : (float)texture.width;
                    float h = (config.height > 0) ? config.height : (float)texture.height;
                    sourceRec = { config.startX, config.startY, w, h };
                }
                Rectangle destRec = { entity.x, entity.y, sourceRec.width * config.scale, sourceRec.height * config.scale };
                Vector2 origin = { destRec.width / 2, destRec.height / 2 };
                DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
            }
        } else {
            DrawCircleLines(static_cast<int>(entity.x), static_cast<int>(entity.y), 20, MAGENTA);
        }
    }

    if (_textures.count(4)) {
        const Texture2D& texture = _textures.at(4);
        for (auto it = _explosions.begin(); it != _explosions.end();) {
            double lifeTime = GetTime() - it->startTime;
            if (lifeTime > 0.5) {
                it = _explosions.erase(it);
            } else {
                int frame = static_cast<int>(lifeTime * 12.0f) % 6;                 
                float startX = 67.0f;
                float startY = 294.0f;
                float frameWidth = 38.0f; 
                float frameHeight = 32.0f;

                Rectangle sourceRec = { startX + frame * frameWidth, startY, frameWidth, frameHeight };
                Rectangle destRec = { it->x, it->y, frameWidth * 2.5f, frameHeight * 2.5f };
                Vector2 origin = { destRec.width / 2.0f, destRec.height / 2.0f };
                
                DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
                ++it;
            }
        }
    }
}

void Renderer::drawChat(const std::vector<std::string>& messages, const std::string& currentInput, bool isActive)
{
    int screenHeight = GetScreenHeight();
    int startY = screenHeight - 150;
    int startX = 20;

    int count = 0;
    for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
        if (count >= 5) break;
        DrawText(it->c_str(), startX, startY - (count * 25), 20, WHITE);
        count++;
    }

    if (isActive) {
        DrawRectangle(startX, startY + 30, 400, 30, Fade(DARKGRAY, 0.8f));
        DrawRectangleLines(startX, startY + 30, 400, 30, LIGHTGRAY);
        DrawText(currentInput.c_str(), startX + 5, startY + 35, 20, WHITE);

        if ((int)(GetTime() * 2) % 2 == 0) {
            DrawText("_", startX + 5 + MeasureText(currentInput.c_str(), 20), startY + 35, 20, WHITE);
        }
    } else {
        DrawText("Press TAB to chat", startX, startY + 35, 15, Fade(LIGHTGRAY, 0.5f));
    }
}

PauseMenuChoice Renderer::drawPauseMenu()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    DrawText("PAUSED", GetScreenWidth() / 2 - MeasureText("PAUSED", 60) / 2, 150, 60, WHITE);

    Vector2 mousePos = GetMousePosition();
    PauseMenuChoice choice = PauseMenuChoice::NONE;

    Rectangle optionsBtn = { (float)GetScreenWidth() / 2 - 125, 320, 250, 50 };
    bool hoverOptions = CheckCollisionPointRec(mousePos, optionsBtn);
    DrawRectangleRec(optionsBtn, hoverOptions ? LIGHTGRAY : GRAY);
    DrawText("Options", optionsBtn.x + (optionsBtn.width - MeasureText("Options", 30)) / 2, optionsBtn.y + 10, 30, BLACK);
    if (hoverOptions && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        choice = PauseMenuChoice::OPTIONS;
    }

    Rectangle quitBtn = { (float)GetScreenWidth() / 2 - 125, 390, 250, 50 };
    bool hoverQuit = CheckCollisionPointRec(mousePos, quitBtn);
    DrawRectangleRec(quitBtn, hoverQuit ? LIGHTGRAY : GRAY);
    DrawText("Quit Game", quitBtn.x + (quitBtn.width - MeasureText("Quit Game", 30)) / 2, quitBtn.y + 10, 30, BLACK);
    if (hoverQuit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        choice = PauseMenuChoice::QUIT;
    }

    DrawText("Press ESC to resume", GetScreenWidth() / 2 - MeasureText("Press ESC to resume", 20) / 2, GetScreenHeight() - 50, 20, LIGHTGRAY);

    return choice;
}

MainMenuChoice Renderer::drawMainMenu()
{
    ClearBackground(BLACK);

    DrawText("R-Type", GetScreenWidth() / 2 - MeasureText("R-Type", 80) / 2, 100, 80, WHITE);

    Vector2 mousePos = GetMousePosition();
    MainMenuChoice choice = MainMenuChoice::NONE;

    Rectangle startBtn = { (float)GetScreenWidth() / 2 - 125, 300, 250, 50 };
    bool hoverStart = CheckCollisionPointRec(mousePos, startBtn);
    DrawRectangleRec(startBtn, hoverStart ? LIGHTGRAY : GRAY);
    DrawText("Start", startBtn.x + (startBtn.width - MeasureText("Start", 30)) / 2, startBtn.y + 10, 30, BLACK);
    if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        choice = MainMenuChoice::START;
    }

    Rectangle optionsBtn = { (float)GetScreenWidth() / 2 - 125, 370, 250, 50 };
    bool hoverOptions = CheckCollisionPointRec(mousePos, optionsBtn);
    DrawRectangleRec(optionsBtn, hoverOptions ? LIGHTGRAY : GRAY);
    DrawText("Options", optionsBtn.x + (optionsBtn.width - MeasureText("Options", 30)) / 2, optionsBtn.y + 10, 30, BLACK);
    if (hoverOptions && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        choice = MainMenuChoice::OPTIONS;
    }

    return choice;
}

bool Renderer::drawOptionsMenu(std::map<std::string, int>& keybinds)
{
    ClearBackground(BLACK);

    DrawText("Options - Keybinds", 50, 50, 40, WHITE);

    Vector2 mousePos = GetMousePosition();
    bool backPressed = false;

    Rectangle backBtn = { 50, (float)GetScreenHeight() - 100, 150, 50 };
    bool hoverBack = CheckCollisionPointRec(mousePos, backBtn);
    DrawRectangleRec(backBtn, hoverBack ? LIGHTGRAY : GRAY);
    DrawText("Back", backBtn.x + (backBtn.width - MeasureText("Back", 30)) / 2, backBtn.y + 10, 30, BLACK);
    if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (_actionToRemap.has_value()) {
            _actionToRemap.reset();
        } else {
            backPressed = true;
        }
    }

    if (_actionToRemap.has_value()) {
        std::string waitText = "Press a key for " + _actionToRemap.value() + "...";
        DrawText(waitText.c_str(), GetScreenWidth() / 2 - MeasureText(waitText.c_str(), 30) / 2, GetScreenHeight() / 2 - 15, 30, YELLOW);

        int keyPressed = GetKeyPressed();
        if (keyPressed != 0) {
            keybinds[_actionToRemap.value()] = keyPressed;
            _actionToRemap.reset();
        }
    } else {
        int startY = 150;
        for (auto const& [action, key] : keybinds) {
            DrawText(action.c_str(), 100, startY, 30, WHITE);

            const char *keyName = GetKeyName(key);
            DrawText(keyName ? keyName : "N/A", 400, startY, 30, LIGHTGRAY);

            Rectangle changeBtn = { 600, (float)startY - 5, 150, 40 };
            bool hoverChange = CheckCollisionPointRec(mousePos, changeBtn);
            DrawRectangleRec(changeBtn, hoverChange ? SKYBLUE : BLUE);
            DrawText("Change", changeBtn.x + (changeBtn.width - MeasureText("Change", 20)) / 2, changeBtn.y + 10, 20, WHITE);

            if (hoverChange && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                _actionToRemap = action;
            }

            startY += 50;
        }
    }

    return backPressed;
}


int Renderer::drawRoomMenu(const std::vector<RoomInfo>& rooms)
{
    ClearBackground(BLACK);

    DrawText("R-Type - Select Room", 50, 50, 40, WHITE);

    Rectangle createBtn = { 50, 120, 250, 50 };
    Vector2 mousePos = GetMousePosition();
    bool hoverCreate = CheckCollisionPointRec(mousePos, createBtn);

    DrawRectangleRec(createBtn, hoverCreate ? LIGHTGRAY : GRAY);
    DrawText("Create Room", 60, 130, 30, BLACK);

    int action = -1;
    if (hoverCreate && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        action = -2;
    }

    int startY = 200;
    for (const auto& room : rooms) {
        Rectangle roomBtn = { 50, (float)startY, 600, 50 };
        bool hoverRoom = CheckCollisionPointRec(mousePos, roomBtn);

        DrawRectangleRec(roomBtn, hoverRoom ? DARKGRAY : BLACK);
        DrawRectangleLinesEx(roomBtn, 2, WHITE);

        std::string text = "Room " + std::to_string(room.id) + "   Players: " +
                            std::to_string(room.playerCount) + "/" + std::to_string(room.maxPlayers);
        DrawText(text.c_str(), 70, startY + 10, 30, WHITE);

        if (hoverRoom && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            action = room.id;
        }
        startY += 60;
    }

    return action;
}

void Renderer::addExplosion(float x, float y)
{
    _explosions.push_back({x, y, GetTime()});
}

bool Renderer::drawLobby(const LobbyState& lobbyState, uint32_t myPlayerId)
{
    ClearBackground(BLACK);

    DrawText("Lobby - Waiting for players...", 50, 50, 40, WHITE);

    int startY = 150;
    for (const auto& player : lobbyState.players) {
        std::string text = std::string(player.username) + " (ID: " + std::to_string(player.playerId) + ")";
        if (player.playerId == lobbyState.hostId) {
            text += " - HOST";
        }
        DrawText(text.c_str(), 70, startY, 30, WHITE);
        startY += 40;
    }

    bool startGamePressed = false;
    if (myPlayerId == lobbyState.hostId) {
        Rectangle startBtn = { 50, (float)GetScreenHeight() - 100, 250, 50 };
        Vector2 mousePos = GetMousePosition();
        bool hoverStart = CheckCollisionPointRec(mousePos, startBtn);

        DrawRectangleRec(startBtn, hoverStart ? SKYBLUE : BLUE);
        DrawText("Start Game", 60, GetScreenHeight() - 90, 30, WHITE);

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            startGamePressed = true;
        }
    } else {
        DrawText("Waiting for the host to start the game...", 50, GetScreenHeight() - 100, 20, LIGHTGRAY);
    }

    EndDrawing();
    return startGamePressed;
}

bool Renderer::drawUsernameInput(std::string& username)
{
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (username.length() < 31)) {
            username += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!username.empty()) {
            username.pop_back();
        }
    }

    ClearBackground(BLACK);
    DrawText("CHOOSE YOUR USERNAME", GetScreenWidth() / 2 - MeasureText("CHOOSE YOUR USERNAME", 40) / 2, 150, 40, WHITE);

    Rectangle textBox = { (float)GetScreenWidth() / 2 - 200, 300, 400, 50 };
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(username.c_str(), (int)textBox.x + 10, (int)textBox.y + 10, 30, MAROON);

    if (((int)(GetTime() * 2.0f)) % 2 == 0) {
        DrawText("_", (int)textBox.x + 10 + MeasureText(username.c_str(), 30), (int)textBox.y + 15, 30, MAROON);
    }

    DrawText("Press ENTER to confirm", GetScreenWidth() / 2 - MeasureText("Press ENTER to confirm", 20) / 2, 370, 20, GRAY);
    DrawText("Max 31 characters", GetScreenWidth() / 2 - MeasureText("Max 31 characters", 20) / 2, 400, 20, GRAY);

    if (IsKeyPressed(KEY_ENTER) && !username.empty()) {
        return true;
    }

    return false;
}

void Renderer::drawKickedScreen()
{
    ClearBackground(BLACK);
    const char* title = "You have been kicked from the server.";
    const char* subtitle = "Press ENTER or click to exit.";

    int titleWidth = MeasureText(title, 40);
    int subtitleWidth = MeasureText(subtitle, 20);

    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - 40, 40, RED);
    DrawText(subtitle, GetScreenWidth() / 2 - subtitleWidth / 2, GetScreenHeight() / 2 + 20, 20, LIGHTGRAY);
}

const char* Renderer::GetKeyName(int key) {
    switch (key) {
        case KEY_APOSTROPHE:   return "'";
        case KEY_COMMA:        return ",";
        case KEY_MINUS:        return "-";
        case KEY_PERIOD:       return ".";
        case KEY_SLASH:        return "/";
        case KEY_ZERO:         return "0";
        case KEY_ONE:          return "1";
        case KEY_TWO:          return "2";
        case KEY_THREE:        return "3";
        case KEY_FOUR:         return "4";
        case KEY_FIVE:         return "5";
        case KEY_SIX:          return "6";
        case KEY_SEVEN:        return "7";
        case KEY_EIGHT:        return "8";
        case KEY_NINE:         return "9";
        case KEY_SEMICOLON:    return ";";
        case KEY_EQUAL:        return "=";
        case KEY_A:            return "A";
        case KEY_B:            return "B";
        case KEY_C:            return "C";
        case KEY_D:            return "D";
        case KEY_E:            return "E";
        case KEY_F:            return "F";
        case KEY_G:            return "G";
        case KEY_H:            return "H";
        case KEY_I:            return "I";
        case KEY_J:            return "J";
        case KEY_K:            return "K";
        case KEY_L:            return "L";
        case KEY_M:            return "M";
        case KEY_N:            return "N";
        case KEY_O:            return "O";
        case KEY_P:            return "P";
        case KEY_Q:            return "Q";
        case KEY_R:            return "R";
        case KEY_S:            return "S";
        case KEY_T:            return "T";
        case KEY_U:            return "U";
        case KEY_V:            return "V";
        case KEY_W:            return "W";
        case KEY_X:            return "X";
        case KEY_Y:            return "Y";
        case KEY_Z:            return "Z";

        case KEY_SPACE:        return "SPACE";
        case KEY_ESCAPE:       return "ESC";
        case KEY_ENTER:        return "ENTER";
        case KEY_TAB:          return "TAB";
        case KEY_BACKSPACE:    return "BACKSPACE";
        case KEY_INSERT:       return "INSERT";
        case KEY_DELETE:       return "DEL";
        case KEY_RIGHT:        return "RIGHT";
        case KEY_LEFT:         return "LEFT";
        case KEY_DOWN:         return "DOWN";
        case KEY_UP:           return "UP";
        case KEY_PAGE_UP:      return "PAGE UP";
        case KEY_PAGE_DOWN:    return "PAGE DOWN";
        case KEY_HOME:         return "HOME";
        case KEY_END:          return "END";
        case KEY_CAPS_LOCK:    return "CAPS";
        case KEY_SCROLL_LOCK:  return "SCROLL LOCK";
        case KEY_NUM_LOCK:     return "NUM LOCK";
        case KEY_PRINT_SCREEN: return "PRINT SCREEN";
        case KEY_PAUSE:        return "PAUSE";
        case KEY_F1:           return "F1";
        case KEY_F2:           return "F2";
        case KEY_F3:           return "F3";
        case KEY_F4:           return "F4";
        case KEY_F5:           return "F5";
        case KEY_F6:           return "F6";
        case KEY_F7:           return "F7";
        case KEY_F8:           return "F8";
        case KEY_F9:           return "F9";
        case KEY_F10:          return "F10";
        case KEY_F11:          return "F11";
        case KEY_F12:          return "F12";
        case KEY_LEFT_SHIFT:   return "L SHIFT";
        case KEY_LEFT_CONTROL: return "L CTRL";
        case KEY_LEFT_ALT:     return "L ALT";
        case KEY_RIGHT_SHIFT:  return "R SHIFT";
        case KEY_RIGHT_CONTROL:return "R CTRL";
        case KEY_RIGHT_ALT:    return "R ALT";

        default:               return "UNKNOWN";
    }
}