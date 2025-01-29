# Movie Data Fetcher

## Overview

Fetch and explore trending movies effortlessly! The Movie Data Fetcher is a C++ application that retrieves trending movie data from The Movie Database (TMDb) API and presents it in a user-friendly graphical user interface (GUI) built with Dear ImGui.  Leveraging multi-threading, the application ensures a smooth and responsive experience by handling data fetching and GUI rendering concurrently.

## Features

- Fetches trending movie data from TMDb API.
- Displays movie data in a GUI using Dear ImGui.
- Supports filtering, sorting, and saving movie data.
- Uses multi-threading for efficient data fetching and GUI rendering.
- Caches textures to improve performance.

## Technologies Used

* **C++17:** Foundation of the application's codebase.
* **Dear ImGui:**  Provides a flexible and powerful immediate-mode GUI.
* **httplib:** Simplifies HTTP/HTTPS communication with the TMDb API.
* **nlohmann/json:**  Handles JSON parsing for seamless data processing.
* **OpenSSL:** Ensures secure HTTPS connections to the TMDb API.
* **DirectX 11:**  Powers texture rendering within the GUI.


## Getting Started

### Prerequisites

* **C++ Compiler:** Supporting C++17 or later (e.g., g++, clang++, Visual Studio 2019+).
* **CMake:**  For building the project across different platforms.
* **OpenSSL Library:** Required for secure communication.
* **Visual Studio:** 2022 (for Windows).
* **TMDb API Key:** Obtain a free API key from [https://www.themoviedb.org/](https://www.themoviedb.org/).

### Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/ilansou/CppApp.git
   cd CppApp
   ```

2. **Set TMDb API Key (Environment Variable):**

    * **Windows:**
      ```bash
      setx TMDB_API_KEY "your_tmdb_api_key"
      ```

    * **Linux/macOS:**
      ```bash
      export TMDB_API_KEY="your_tmdb_api_key"
      ```
3. **Build (CMake):**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

4. **Run:** Navigate to the `build` directory and execute the generated executable.

## Project Structure

```
MovieDataFetcher/
├── winapp/            # Windows-specific application code
│   └── ConnectedApp/    # Main application code (Windows)
├── shared/           # Shared code between different parts
├── ImGuiSrc/         # Dear ImGui source code
├── stb_image.h       # Image loading library
├── CommonObjects.h    # Shared data structure between threads
├── DownloadThread.h   # Movie data download thread
├── DrawThread.h      # GUI rendering thread
├── GuiMain.h        # GUI initialization and main loop
├── MovieService.h     # Movie-related operations
├── CMakeLists.txt    # CMake build configuration
└── LICENSE           # Project license (MIT)

```


## Multi-threading Architecture

The application employs a multi-threaded architecture to ensure responsiveness:

* **`DownloadThread`:** Downloads movie data asynchronously from the TMDb API.
* **`DrawThread`:** Renders the GUI using Dear ImGui, independent of data fetching.
* **`CommonObjects`:**  A shared data structure facilitates communication between the `DownloadThread` and `DrawThread`.


## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bug reports, feature requests, or suggestions.
