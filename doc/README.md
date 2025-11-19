# RType - C++ Project

This project uses **C++20**, **Raylib**, and **Conan** for dependency management.  
This guide explains how to install the required tools and build the project on Linux.

---

## Prerequisites

### 1. Install Conan

Conan is the C/C++ package manager.  

```bash
# Install Conan
pipx install conan

# Verify installation
conan --version
```

### 2. Install CMake

CMake ≥ 3.15 is required.

```bash
# Ubuntu / Debian
sudo apt install cmake -y

# Fedora
sudo dnf install cmake -y

# Verify installation
cmake --version
```

### 3. Build project

```bash
# On Linux
./build.sh

# On Windows
./build.bat
```
