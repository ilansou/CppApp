// ConnectedApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <mutex>
#include "CommonObjects.h"
#include "DrawThread.h"
#include "DownloadThread.h"

// Mutex for synchronizing access to shared resources
std::mutex mtx; 

int main() {
    CommonObjects common;
    DrawThread draw;
    auto draw_th = std::jthread([&] {draw(common); });
    DownloadThread down;
    auto down_th = std::jthread([&] {down(common); });
    down.SetUrl("http://....");
    std::cout << "running...\n";

	return 0;
}
