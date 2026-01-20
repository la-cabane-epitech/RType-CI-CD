# R-Type

This project is a C++ implementation of the classic shoot'em up game R-Type, featuring a client-server architecture for multiplayer gameplay. It is built with modern C++20, using Raylib for graphics and Conan for dependency management.

---

## Table of Contents

1.  [About The Project](#about-the-project)
2.  [Getting Started](#getting-started)
    -   [Prerequisites](#prerequisites)
    -   [Building](#building)
3.  [Usage](#usage)
4.  [Documentation](#documentation)
5.  [Contributing](#contributing)

## About The Project

This R-Type clone is designed with a robust client-server model:
-   **Server**: Manages game logic, entity states, and player connections.
-   **Client**: Renders the game using the **Raylib** library and sends user input to the server.
-   **Network**: Uses a dual-protocol approach:
    -   **TCP** for reliable session management (connecting, lobbies).
    -   **UDP** for low-latency real-time gameplay updates.

The project is cross-platform and can be built on both Windows and Linux.

## Getting Started

Follow these steps to get a local copy up and running.

### Prerequisites

You will need the following tools installed on your system:

1.  **C++20 Compiler**:
    -   **Linux**: GCC or Clang.
    -   **Windows**: Visual Studio 2022 (with C++ workload).
2.  **CMake** (version 3.15 or higher):
    ```bash
    # Ubuntu / Debian
    sudo apt install cmake
    # Fedora
    sudo dnf install cmake
    # Windows: Download from https://cmake.org/download/
    ```
3.  **Conan** (C/C++ Package Manager):
    ```bash
    pip install conan
    # Or using pipx (recommended)
    pipx install conan
    ```

### Building

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/EpitechPGE3-2025/RType-CI-CD.git
    cd RType-CI-CD
    ```

2.  **Run the build script:**
    The provided scripts will automatically use Conan to fetch dependencies (like Raylib and Asio) and then use CMake to build the project.

    -   On **Linux**:
        ```bash
        ./build.sh
        ```
    -   On **Windows** (from a developer command prompt):
        ```bash
        ./build.bat
        ```

    The compiled executables (`rtype_client` / `rtype_client.exe` and `rtype_server` / `rtype_server.exe`) will be placed in the root directory of the project.

## Usage

1.  **Start the server:**
    The server requires a port to listen on.
    ```bash
    ./rtype_server
    ```

2.  **Start the client:**
    The client needs the server's IP address and port to connect.
    ```bash
    ./rtype_client <server_ip>
    ```

## Documentation

The network protocol is detailed in the RFC_Protocol.md file. It specifies all TCP and UDP packet structures used for communication.

## Contributing

Contributions are welcome! Please read our Contribution Guidelines for details on our code style, commit conventions, and workflow.
