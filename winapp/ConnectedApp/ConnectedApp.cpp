// ConnectedApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <mutex>
#include "CommonObjects.h"
#include "DrawThread.h"
#include "DownloadThread.h"

std::mutex mtx;

int main() {
    CommonObjects common;
    DrawThread draw;
    auto draw_th = std::jthread([&] {draw(common); });
    DownloadThread down;
    auto down_th = std::jthread([&] {down(common); });
    down.SetUrl("http://....");
    std::cout << "running...\n";
    /*down_th.join();
    draw_th.join();*/

	return 0;
}

//int main() {
//    CommonObjects common;
//    DrawThread draw;
//    DownloadThread down;
//
//    // Start the download thread
//    auto down_th = std::jthread([&] { down(common); });
//
//    // Wait for the download thread to finish
//    down_th.join();
//
//    // Check if data is ready
//    if (common.data_ready) {
//        std::cout << "Movies fetched successfully!\n";
//        std::cout << "Number of movies: " << common.movies.size() << "\n\n";
//
//        // Print details of each movie
//        for (const auto& movie : common.movies) {
//            std::cout << "Title: " << movie.title << "\n";
//            std::cout << "Overview: " << movie.overview << "\n";
//            std::cout << "Release Date: " << movie.release_date << "\n";
//            std::cout << "Popularity: " << movie.popularity << "\n";
//            std::cout << "Vote Average: " << movie.vote_average << "\n";
//            std::cout << "Vote Count: " << movie.vote_count << "\n";
//            std::cout << "Poster Path: " << movie.poster_path << "\n";
//            std::cout << "----------------------------------------\n";
//        }
//    }
//    else {
//        std::cout << "Failed to fetch movies or no data available.\n";
//    }
//
//    // Start the draw thread (if needed)
//    auto draw_th = std::jthread([&] { draw(common); });
//    draw_th.join();
//
//    return 0;
//}

