#pragma once
#include "DownloadThread.h"
#include "CommonObjects.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Movie, title, overview, release_date, poster_path, popularity, vote_count, vote_average)


std::string DownloadThread::GetPosterFilename(const std::string& poster_path) {
    // Extract filename (everything after the last '/')
    size_t lastSlash = poster_path.find_last_of('/');
    std::string filename = (lastSlash == std::string::npos) ? poster_path : poster_path.substr(lastSlash + 1);
    return filename;
}

void DownloadThread::operator()(CommonObjects& common) {
    httplib::Client cli("api.themoviedb.org");

    const std::string API_KEY = "fd05a4fdf85e914ec224c2016dc73bd2";

    // Set headers for TMDb API
    httplib::Headers headers = {
        {"Authorization", "Bearer " + API_KEY},
        {"Accept", "application/json"}
    };

    auto res = cli.Get("/3/trending/movie/week?api_key=" + API_KEY, headers);

    if (res && res->status == 200) {
        auto json_result = nlohmann::json::parse(res->body);
        common.movies = json_result["results"];

        if (!common.movies.empty()) {
            common.data_ready = true;

            // Create a directory to save posters
            std::string poster_dir = "posters";
            if (!std::filesystem::exists(poster_dir)) {
                std::filesystem::create_directory(poster_dir);
            }

            httplib::Client img_cli("https://image.tmdb.org");
            // Download and save posters
            for (auto& movie : common.movies) {
                if (!movie.poster_path.empty()) {
                    std::string poster_url = "/t/p/w500" + movie.poster_path; // Full URL for the poster
                    std::string local_path = poster_dir + "/" + GetPosterFilename(movie.poster_path); // Remove leading slash

                    std::cout << "Downloading poster: " << poster_url << std::endl;
                    // Download the poster
                    auto poster_res = img_cli.Get(poster_url.c_str());
                    if (poster_res && poster_res->status == 200) {
                        std::ofstream outfile(local_path, std::ios::binary);
                        outfile.write(poster_res->body.data(), poster_res->body.size());
                        outfile.close();

                        // Update the poster_path to the local path
                        movie.poster_path = local_path;
                    }
                    else {
                        std::cerr << "Failed to download poster: " << poster_url << " Status: " << poster_res->status << std::endl;
                        movie.poster_path.clear(); // Clear the path if download fails
                    }
                }
            }

            // Update the movie map
            for (const auto& movie : common.movies) {
                common.movie_map[movie.title] = movie;
            }

            common.FilterMovies(); // Filter movies initially
        }
    }
    else {
        std::cerr << "Failed to fetch movies. Error code: " << res->status << std::endl;
    }
}

void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}
