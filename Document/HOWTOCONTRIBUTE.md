# How to Contribute to R-Type

First off, thank you for considering contributing to R-Type! We welcome any help, from fixing bugs to implementing new features. This document provides guidelines to ensure a smooth and effective contribution process.

## Table of Contents
1. [Getting Started](#getting-started)
2. [Contribution Workflow](#contribution-workflow)
3. [Coding Style and Conventions](#coding-style-and-conventions)
4. [Commit Message Guidelines](#commit-message-guidelines)
5. [Protocol Changes](#protocol-changes)
6. [Reporting Bugs](#reporting-bugs)

## Getting Started

Before you begin, please ensure you have set up your development environment as described in the [README.md](README.md). This includes installing **Conan**, **CMake**, and a **C++20 compatible compiler**.

1.  **Fork & Clone the repository:**
    ```bash
    git clone https://github.com/YOUR_USERNAME/RType-CI-CD.git
    cd RType-CI-CD
    ```

2.  **Build the project:**
    Use the provided build scripts to compile the client and server. This will also install all necessary dependencies via Conan.

    *   On **Linux**:
        ```bash
        ./build.sh
        ```
    *   On **Windows**:
        ```bash
        ./build.bat
        ```
    The executables (`rtype_client` and `rtype_server`) will be generated in the root directory.

## Contribution Workflow

We use the standard "Fork & Pull" model.

1.  **Create a new branch** for your feature or bugfix. Please use a descriptive name.
    *   For features: `feat/short-feature-description` (e.g., `feat/add-new-enemy-type`)
    *   For bugfixes: `fix/short-bug-description` (e.g., `fix/player-desync-on-shoot`)
    ```bash
    git checkout -b feat/my-awesome-feature
    ```

2.  **Make your changes.** Write clean, commented, and tested code.

3.  **Commit your changes** following our Commit Message Guidelines.

4.  **Push your branch** to your fork:
    ```bash
    git push origin feat/my-awesome-feature
    ```

5.  **Open a Pull Request (PR)** against the `main` branch of the main repository.
    *   Provide a clear title and a detailed description of your changes.
    *   If your PR addresses an open issue, link it (e.g., "Closes #42").

## Coding Style and Conventions

Consistency is key. Please adhere to the following conventions.

### Naming
-   **Types (Classes, Structs, Enums):** `PascalCase` (e.g., `PlayerInputPacket`, `TCPMessageType`).
-   **Functions & Methods:** `camelCase` (e.g., `getPlayerState`, `sendPacket`).
-   **Variables (including members):** `camelCase` (e.g., `playerId`, `entityCount`).
-   **Constants & Enum Members:** `UPPER_SNAKE_CASE` (e.g., `MAX_UDP_PACKET_SIZE`, `PLAYER_INPUT`).

### Formatting
-   **Indentation:** 4 spaces.
-   **Braces:** Use Allman style (braces on their own line).
    ```cpp
    void myFunction()
    {
        if (condition)
        {
            // ...
        }
    }
    ```
-   **Pointers/References:** Place the `*` or `&` next to the type (e.g., `int* ptr`, not `int *ptr`).

### Documentation
-   **Public APIs must be documented.** Use Doxygen-style comments in all header files (`.hpp`).
-   Provide a `@brief` for every class, struct, enum, and function.
-   Use `@param` for function parameters and `@return` for return values.

    ```cpp
    /**
     * @brief Sends a packet over the network.
     * @param packet The data to send.
     * @param size The size of the data.
     * @return True if the send was successful, false otherwise.
     */
    bool sendPacket(const char* packet, size_t size);
    ```

## Commit Message Guidelines

We follow the Conventional Commits specification. This helps in creating an explicit commit history and automating changelog generation.

Each commit message consists of a **header**, a **body**, and a **footer**.

```
<type>(<scope>): <subject>
<BLANK LINE>
<body>
<BLANK LINE>
<footer>
```

-   **Type**: Must be one of the following:
    -   `feat`: A new feature.
    -   `fix`: A bug fix.
    -   `docs`: Documentation only changes.
    -   `style`: Changes that do not affect the meaning of the code (white-space, formatting, etc).
    -   `refactor`: A code change that neither fixes a bug nor adds a feature.
    -   `perf`: A code change that improves performance.
    -   `test`: Adding missing tests or correcting existing tests.
    -   `build`: Changes that affect the build system or external dependencies (Conan, CMake).
    -   `ci`: Changes to our CI configuration files and scripts.

-   **Scope** (optional): The part of the codebase affected (e.g., `client`, `server`, `network`, `protocol`).

-   **Subject**: A short, imperative-tense description of the change.

**Examples:**
```
feat(server): Implement lobby creation logic
```
```
fix(protocol): Correct byte alignment in PlayerStatePacket
```
```
docs(rfc): Update UDP packet table with GLOBAL_STATE_SYNC
```

## Protocol Changes

The network protocol is the backbone of the game. Any changes must be handled with extreme care.

1.  Modify the packet structures in `Include/Protocole/ProtocoleTCP.hpp` or `Include/Protocole/ProtocoleUDP.hpp`.
2.  **Crucially**, you **must** update the documentation to reflect your changes:
    -   `Document/RFC_Protocol.md`
    -   `Document/rfc.txt`

Failure to update the documentation will result in the PR being rejected. Remember to keep all network structures packed using `#pragma pack(push, 1)`.

## Reporting Bugs

If you find a bug, please open an issue on GitHub. Provide as much detail as possible to help us reproduce and fix it:

-   **Title**: A clear and descriptive title.
-   **Steps to Reproduce**: Detailed steps to trigger the bug.
-   **Expected Behavior**: What you expected to happen.
-   **Actual Behavior**: What actually happened.
-   **System Information**: Your OS (e.g., Windows 11, Ubuntu 22.04), compiler, etc.
-   **Screenshots/Logs**: If applicable, include them to provide more context.