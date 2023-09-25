# Bluemetal Engine

![bluemetallogo](Images/bluemetal_logo.png)
![LinuxBuild](https://github.com/cadenmiller/bluemetal/actions/workflows/cmake.yml/badge.svg)
![CppVer](https://img.shields.io/badge/C%2B%2B-20-blue)
![VulkanVer](https://img.shields.io/badge/Vulkan-1.3-red)

bluemetal is a powerful and versatile game engine built using modern C++17/20 features. It incorporates cutting-edge technologies such as Vulkan Renderer, a custom configuration language called Noodle, Dear ImGui for user interface, and open-source libraries like SDL2 and VMA.

## Features

- **Use of Newer C++17/20 Features:** bluemetal Engine takes advantage of the latest features provided by C++17/20, offering improved performance, better code organization, and enhanced productivity.

- **Vulkan Renderer:** bluemetal uses Vulkan for backend rendering and graphics operations.

- **Dear ImGui:** bluemetal engine integrates Dear ImGui for some basic development and frontend UI.

- **Others Open Source:** Many open source libraries are used in the creation of bluemetal.

#### Warning: Still in development, not production ready.


## Getting Started

Follow these steps to get started with bluemetal:

1. Clone the repository:
```
git clone https://github.com/cullvox/bluemetal.git
```

2. Install the necessary dependencies:
   - SDL2: Follow the installation instructions specific to your operating system from the official SDL website.
   - Vulkan SDK: LunarG has this for Windows, MacOS, Ubuntu, and more, some distros require specific instructions.
   - On NixOS, these packages are included in the shell.nix file.

3. Build the engine:
   - Navigate to the cloned repository's root directory.
   - Run the build command specific to your platform (e.g., `make`, `msbuild`, `cmake`, etc.).

4. Start developing!

## Contributing
Contributions to bluemetal are welcome! If you find any issues or have suggestions for improvements, please open an issue in the [Issue Tracker](https://github.com/cadenmiller/bluemetal/issues).

If you wish to contribute code, fork the repository, make your changes, and submit a pull request. Ensure that your code follows the project's coding conventions and includes appropriate tests and documentation.

## License

bluemetal is released under the [MIT License](LICENSE). Feel free to use, modify, and distribute the engine according to the terms of the license.

## Acknowledgments

Bluemetal Engine uses the following projects:

- [SDL2](https://www.libsdl.org/)
- [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [{FMT}](https://github.com/fmtlib/fmt)
