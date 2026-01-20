## Library Comparison: Raylib vs. SFML vs. SDL2 vs. OpenGL

Choosing the right graphics library is a crucial decision for any game or multimedia project. This project uses **Raylib** for its simplicity and ease of use. Here's a brief comparison with other popular alternatives, demonstrating a simple "Hello World" and a "Moving Circle" example for each.

### 1. Raylib

**Philosophy**: A simple and easy-to-use library to enjoy C/C++ programming. It's inspired by Borland BGI and XNA, and it's great for prototyping, tooling, and education.

*   **Pros**: Extremely simple API, all-in-one (graphics, audio, input), no external dependencies needed for the core library, great documentation.
*   **Cons**: Less flexible than lower-level APIs, C-style API might not appeal to all C++ developers.

#### Hello World (Raylib)
```cpp
#include "raylib.h"

int main() {
    InitWindow(800, 600, "Raylib - Hello World");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

#### Moving Circle (Raylib)
```cpp
#include "raylib.h"

int main() {
    InitWindow(800, 600, "Raylib - Moving Circle");
    SetTargetFPS(60);

    Vector2 circlePos = { 400, 300 };
    float speed = 200.0f; // pixels per second

    while (!WindowShouldClose()) {
        // Update
        if (IsKeyDown(KEY_RIGHT)) circlePos.x += speed * GetFrameTime();
        if (IsKeyDown(KEY_LEFT))  circlePos.x -= speed * GetFrameTime();
        if (IsKeyDown(KEY_UP))    circlePos.y -= speed * GetFrameTime();
        if (IsKeyDown(KEY_DOWN))  circlePos.y += speed * GetFrameTime();

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circlePos, 50, MAROON);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### 2. SFML (Simple and Fast Multimedia Library)

**Philosophy**: An object-oriented C++ library that provides a simple interface to various components of your PC (graphics, audio, network, etc.).

*   **Pros**: Clean C++ API, modular (use only what you need), good community support.
*   **Cons**: Can be slightly more verbose than Raylib for simple tasks. Additionally, since a core goal of this project was to build the network layer from scratch, SFML's built-in networking module made it less suitable for our specific learning objectives.

#### Hello World (SFML)
```cpp
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML - Hello World");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) { /* error handling */ }

    sf::Text text("Hello, World!", font, 50);
    text.setPosition(190, 200);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(text);
        window.display();
    }

    return 0;
}
```

#### Moving Circle (SFML)
```cpp
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML - Moving Circle");
    window.setFramerateLimit(60);

    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color::Red);
    circle.setOrigin(50.f, 50.f); // Center the origin
    circle.setPosition(400, 300);

    float speed = 300.0f;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  circle.move(-speed * dt, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) circle.move(speed * dt, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    circle.move(0, -speed * dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  circle.move(0, speed * dt);

        window.clear();
        window.draw(circle);
        window.display();
    }

    return 0;
}
```

### 3. SDL2 (Simple DirectMedia Layer)

**Philosophy**: A low-level, cross-platform library providing access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D. It's often used as a base layer for game engines.

*   **Pros**: Extremely portable, gives a lot of control, robust and battle-tested.
*   **Cons**: C-style API, very verbose. Drawing shapes or rendering text requires extra libraries (like `SDL_gfx`, `SDL_ttf`) or manual implementation.

#### Hello World (SDL2 with SDL_ttf)
```cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("SDL2 - Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Hello, World!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { 190, 200, surface->w, surface->h };

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
```

#### Moving Circle (SDL2)
SDL has no built-in function to draw a circle. You must implement it yourself. Here is a simple, non-optimized example.

```cpp
#include <SDL2/SDL.h>
#include <cmath>

void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL2 - Moving Circle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    float x = 400, y = 300;
    float speed = 200.0f;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    bool running = true;
    while (running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT])  x -= speed * deltaTime;
        if (state[SDL_SCANCODE_RIGHT]) x += speed * deltaTime;
        if (state[SDL_SCANCODE_UP])    y -= speed * deltaTime;
        if (state[SDL_SCANCODE_DOWN])  y += speed * deltaTime;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        DrawCircle(renderer, (int)x, (int)y, 50);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```

### 4. OpenGL (Open Graphics Library)

**Philosophy**: A low-level, cross-platform graphics API. It is not a library, but a specification implemented by graphics card drivers. It gives you direct control over the GPU.

*   **Pros**: Maximum performance and flexibility, industry standard for 3D graphics.
*   **Cons**: Extremely steep learning curve. Requires you to manage everything: window creation (with another library like GLFW, SDL), input, matrix math, and shader programming (in GLSL). Simple tasks are very complex.

The following examples are complete but require libraries like **GLFW** (for windowing), **GLAD** (for loading OpenGL functions), and **FreeType** (for fonts) to be properly linked. The code is intentionally verbose to illustrate the low-level control OpenGL provides compared to libraries like Raylib.

#### Hello World (OpenGL with FreeType)
Rendering text in modern OpenGL is a multi-step process involving loading font glyphs into textures and rendering them on individual quads.

```cpp
// Requires linking: GLFW, GLAD, FreeType
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <map>
#include <string>

// Shaders for text rendering
const char* textVertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)glsl";

const char* textFragmentShaderSource = R"glsl(
#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D text;
uniform vec3 textColor;
void main() {
    // The 'r' channel of the texture holds the alpha value of the glyph
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)glsl";

// A simplified orthographic projection matrix function. A real project would use a math library like GLM.
void ortho(float* mat, float left, float right, float bottom, float top) {
    mat[0] = 2.0f / (right - left); mat[4] = 0.0f; mat[8] = 0.0f; mat[12] = -(right + left) / (right - left);
    mat[1] = 0.0f; mat[5] = 2.0f / (top - bottom); mat[9] = 0.0f; mat[13] = -(top + bottom) / (top - bottom);
    mat[2] = 0.0f; mat[6] = 0.0f; mat[10] = -1.0f; mat[14] = 0.0f;
    mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
}

// Data for a single character glyph
struct Character { unsigned int TextureID; int SizeX, SizeY; int BearingX, BearingY; unsigned int Advance; };
std::map<char, Character> Characters;
unsigned int textVAO, textVBO;

// Function to render a line of text
void RenderText(unsigned int shader, std::string text, float x, float y, float scale, float* color) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);
    for (char c : text) {
        Character ch = Characters[c];
        float xpos = x + ch.BearingX * scale;
        float ypos = y - (ch.SizeY - ch.BearingY) * scale;
        float w = ch.SizeX * scale;
        float h = ch.SizeY * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f }, { xpos,     ypos,       0.0f, 1.0f }, { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f }, { xpos + w, ypos,       1.0f, 1.0f }, { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale; // Advance is 1/64 pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {
    // 1. Init GLFW, GLAD, and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL - Hello World", NULL, NULL);
    if (!window) { /* error */ return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { /* error */ return -1; }

    // 2. Compile shaders and link program
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &textVertexShaderSource, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &textFragmentShaderSource, NULL); glCompileShader(fs);
    unsigned int textShader = glCreateProgram(); glAttachShader(textShader, vs); glAttachShader(textShader, fs); glLinkProgram(textShader); glDeleteShader(vs); glDeleteShader(fs);

    // 3. Configure OpenGL state for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float projection[16];
    ortho(projection, 0.0f, 800.0f, 0.0f, 600.0f);
    glUseProgram(textShader);
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, projection);

    // 4. Init FreeType and load font glyphs into textures
    FT_Library ft; if (FT_Init_FreeType(&ft)) { /* error */ return -1; }
    FT_Face face; if (FT_New_Face(ft, "arial.ttf", 0, &face)) { /* error */ return -1; }
    FT_Set_Pixel_Sizes(face, 0, 24);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        unsigned int texture; glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Characters[c] = { texture, (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top, (unsigned int)face->glyph->advance.x };
    }
    FT_Done_Face(face); FT_Done_FreeType(ft);

    // 5. Configure VAO/VBO for rendering quads
    glGenVertexArrays(1, &textVAO); glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindVertexArray(0);

    // 6. Main Loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.98f, 0.98f, 0.98f, 1.0f); // RAYWHITE
        glClear(GL_COLOR_BUFFER_BIT);
        float textColor[] = { 0.76f, 0.76f, 0.76f }; // LIGHTGRAY
        RenderText(textShader, "Hello, World!", 190.0f, 500.0f, 1.0f, textColor);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
```

#### Moving Circle (OpenGL)
Drawing a shape involves defining its vertices, sending them to the GPU, and writing shaders to position and color them. Movement is handled by updating a `uniform` variable in the shader each frame.

```cpp
// Requires linking: GLFW, GLAD
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

// Shaders for the circle
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 u_offset;
void main() {
    gl_Position = vec4(aPos.x + u_offset.x, aPos.y + u_offset.y, 0.0, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.76f, 0.15f, 0.18f, 1.0); // Maroon color
}
)glsl";

int main() {
    // 1. Init GLFW, GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL - Moving Circle", NULL, NULL);
    if (!window) { /* error */ return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { /* error */ return -1; }

    // 2. Compile shaders
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertexShaderSource, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragmentShaderSource, NULL); glCompileShader(fs);
    unsigned int shaderProgram = glCreateProgram(); glAttachShader(shaderProgram, vs); glAttachShader(shaderProgram, fs); glLinkProgram(shaderProgram); glDeleteShader(vs); glDeleteShader(fs);

    // 3. Create circle vertices (as a polygon)
    std::vector<float> vertices;
    const int numSegments = 50;
    const float radius = 0.16f; // Radius in normalized device coordinates (-1 to 1)
    const float PI = 3.1415926f;
    vertices.push_back(0.0f); vertices.push_back(0.0f); // Center point for TRIANGLE_FAN
    for (int i = 0; i <= numSegments; ++i) {
        float angle = i * 2.0f * PI / numSegments;
        vertices.push_back(cos(angle) * radius);
        // Correct for aspect ratio to make the circle not look like an ellipse
        vertices.push_back(sin(angle) * radius * (800.0f / 600.0f));
    }

    // 4. Send vertex data to GPU
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 5. Main Loop
    float circleX = 0.0f, circleY = 0.0f;
    float speed = 0.8f; // In NDC units per second
    double lastFrame = 0.0;
    int offsetLocation = glGetUniformLocation(shaderProgram, "u_offset");

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        float deltaTime = static_cast<float>(currentFrame - lastFrame);
        lastFrame = currentFrame;

        // Input
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) circleX += speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  circleX -= speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    circleY += speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  circleY -= speed * deltaTime;

        // Render
        glClearColor(0.98f, 0.98f, 0.98f, 1.0f); // Raylib's RAYWHITE
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glUniform2f(offsetLocation, circleX, circleY);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (int)vertices.size() / 2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 6. Cleanup
    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
```

## Documentation

The network protocol is detailed in the RFC_Protocol.md file. It specifies all TCP and UDP packet structures used for communication.
