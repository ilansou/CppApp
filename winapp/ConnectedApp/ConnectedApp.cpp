#include <iostream>
#include <thread>
#include <mutex>
#include "CommonObjects.h"
#include "DrawThread.h"
#include "DownloadThread.h"

std::mutex mtx;

int main() {
    try {
        CommonObjects common;
        DrawThread draw;
        auto draw_th = std::jthread([&] { draw(common); });
        DownloadThread down;
        auto down_th = std::jthread([&] { down(common); });

        down.SetUrl("http://....");
        std::cout << "running...\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
    }

    return 0;
}