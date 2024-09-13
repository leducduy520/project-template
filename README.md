# 🎮 C++ Project Template

Welcome to my personal C++ project template repository! This project is designed to be a comprehensive starting point for C++ projects, covering various topics including [CMake](https://cmake.org/), [C++](https://isocpp.org/), [Python](https://www.python.org/), [Docker](https://www.docker.com/), [GitHub Actions](https://github.com/features/actions), and code formatting tools such as [CMake format](https://github.com/cheshirekow/cmake_format), [Clang format](https://clang.llvm.org/docs/ClangFormat.html), and [Clang tidy](https://clang.llvm.org/extra/clang-tidy/). Additionally, it includes documentation generation with [Doxygen](https://www.doxygen.nl/) and [Graphviz](https://graphviz.org/).

## 🚀 Project Overview

This template provides a solid foundation for developing C++ projects with modern practices and tools. Below is an overview of the main components in this project:

### 1. **📂 App Folder**
The `app` folder contains the files necessary to build a **game manager**. This game manager is designed to dynamically load different games at runtime and works seamlessly across both UNIX and Windows systems. It serves as the main program that orchestrates the game-loading process.

### 2. **⚙️ CMake Folder**
The `cmake` folder holds all the [CMake](https://cmake.org/) configuration files. These files are used to configure the build system, making it easy to manage dependencies, compile the project, and link it with external libraries.

### 3. **📝 Docs Folder**
The `docs` folder contains the [Doxygen](https://www.doxygen.nl/) configuration file used to generate project documentation. Additionally, it houses the **Graphviz** folder for visual diagrams and the **HTML** folder, which stores the generated documentation in an HTML format for easy viewing and navigation.

### 4. **🕹️ Src Folder**
The `src` folder contains the source code for the games that can be loaded by the game manager. These games are built using the **[SFML](https://www.sfml-dev.org/)** library for rendering the UI and the **[Mongo C++ Driver](https://mongodb.github.io/mongo-cxx-driver/)** to connect with a database. The dynamic loading of games adds flexibility to the game manager.

### 5. **🧪 Test Folder**
The `test` folder is dedicated to unit testing. It contains tests written using **[Catch2](https://github.com/catchorg/Catch2)** and utilizes **CTest** to run these tests as part of the build process. This ensures the reliability of both the game manager and the dynamically loaded games.

### 6. **🛠️ Code Formatting Tools**
This project includes several configuration files to ensure consistent coding standards and style across the entire codebase:

- **[`.clang-format`](https://clang.llvm.org/docs/ClangFormat.html)**: Defines the coding style for formatting C++ code. It ensures a uniform format for things like indentation, spacing, and line breaks.
  
- **[`.clang-tidy`](https://clang.llvm.org/extra/clang-tidy/)**: This file configures **Clang-Tidy**, which performs static analysis on C++ code and checks for potential code errors, performance issues, and style violations.

- **[`cmake-format.yaml`](https://github.com/cheshirekow/cmake_format)**: Configures the formatting rules for **CMake** files. It ensures consistency in how CMake scripts are formatted, following best practices for readability and maintainability.

### 7. **🐋 Dockerfile**
The `Dockerfile` included in this project allows for containerization, enabling a consistent development environment across different systems. It defines the setup process for building and running the project in an isolated [Docker](https://www.docker.com/) container. This ensures that dependencies, libraries, and system configurations remain consistent for all developers and users.

## 🛠️ Key Technologies

- **C++ (C++11 and later)**: The main language used for the project.
- **[CMake](https://cmake.org/)**: For building and managing project dependencies.
- **[Docker](https://www.docker.com/)**: To containerize the project and ensure consistent development environments.
- **[GitHub Actions](https://github.com/features/actions)**: To automate CI/CD pipelines.
- **[SFML](https://www.sfml-dev.org/)**: A graphics library for creating the UI of the games.
- **[Mongo C++ Driver](https://mongodb.github.io/mongo-cxx-driver/)**: To manage game-related data through a database.
- **[Catch2](https://github.com/catchorg/Catch2)**: A modern C++ test framework used in combination with CTest.
- **[Doxygen](https://www.doxygen.nl/)** & **[Graphviz](https://graphviz.org/)**: For generating documentation and visual representations of the project.

## 💻 Getting Started

### 🔨 Build Instructions
Follow these steps to build the project:

1. **Build the Mongo C++ Driver**
   - **Windows**: 
     ```bash
     make mongo-msvc
     ```
   - **Ubuntu**: 
     ```bash
     make mongo-ubuntu
     ```

2. **Build the SFML Library**
   - **Windows**: 
     ```bash
     make sfml-msvc
     ```
   - **Ubuntu**: 
     ```bash
     make sfml-ubuntu
     ```

3. **Configure the Main Project**
   - **Windows**: 
     ```bash
     make configure PRESET=msvc
     ```
   - **Ubuntu**: 
     ```bash
     make configure PRESET=ubuntu
     ```

### 🗂️ Default Folders
- **Default Build Folder:**
   - **Windows**: `./out/msvc/build`
   - **Ubuntu**: `./out/ubuntu/build`
  
- **Default Install Folder:**
   - **Windows**: `./out/msvc/install`
   - **Ubuntu**: `./out/ubuntu/install`

## 🤝 Contribution
Feel free to fork this repository and contribute by submitting pull requests. Whether it's bug fixes, feature additions, or improving the documentation, all contributions are welcome!

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

🎉 **Happy coding!**
