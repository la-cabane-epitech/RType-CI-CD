# RType - C++ Project

This project uses **C++20**, **Raylib**, and **Conan** for dependency management.  
This guide explains how to install the required tools and build the project on Linux.

---

## Prerequisites

### 1. Install Conan

Conan is the C/C++ package manager.  

```bash
# Install pip if needed
sudo apt update
sudo apt install python3-pip -y

# Install Conan
pip install --user conan

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

