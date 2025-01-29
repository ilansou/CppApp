#pragma once
#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <future>

extern std::mutex mtx;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Movie, title, overview, release_date, poster_path, popularity, vote_count, vote_average)

// Extracts the filename from the poster URL
std::string DownloadThread::GetPosterFilename(const std::string& poster_path) {
    size_t lastSlash = poster_path.find_last_of('/');
    return (lastSlash == std::string::npos) ? poster_path : poster_path.substr(lastSlash + 1);
}

// Downloads a movie poster and saves it locally
void DownloadThread::DownloadPoster(Movie& movie, const std::string& poster_dir, CommonObjects& common) {
    if (!movie.poster_path.empty()) {
        std::string poster_url = "/t/p/w500" + movie.poster_path;
        std::filesystem::path local_path = std::filesystem::path(poster_dir) / GetPosterFilename(movie.poster_path);

        httplib::Client img_cli("https://image.tmdb.org");
        auto poster_res = img_cli.Get(poster_url.c_str());

        if (poster_res && poster_res->status == 200) {
            std::ofstream outfile(local_path, std::ios::binary);
            if (outfile) {
                outfile.write(poster_res->body.data(), poster_res->body.size());
                outfile.close();
                movie.poster_path = local_path.string();
            }
            else {
                std::cerr << "Failed to save poster: " << local_path << std::endl;
                movie.poster_path.clear();
            }
        }
        else {
            std::cerr << "Failed to download poster: " << poster_url;
            if (poster_res) {
                std::cerr << " Status: " << poster_res->status;
            }
            else {
                std::cerr << " (Request failed: nullptr response)";
            }
            std::cerr << std::endl;
            movie.poster_path.clear();
        }

        // Update the movie map with thread safety
        {
            std::lock_guard<std::mutex> lock(mtx);
            common.movie_map[movie.title] = movie;
        }
    }
}

// Thread function to fetch movie list and download posters
void DownloadThread::operator()(CommonObjects& common) {
    common.data_ready = false;
    MovieService movieService(common);

    httplib::Client cli("https://api.themoviedb.org");

    const std::string API_KEY = "fd05a4fdf85e914ec224c2016dc73bd2";

    // Set headers for TMDb API request
    httplib::Headers headers = {
        {"Authorization", "Bearer " + API_KEY},
        {"Accept", "application/json"}
    };

    auto res = cli.Get(("/3/trending/movie/week?api_key=" + API_KEY + "&page=" + std::to_string(common.page)).c_str(), headers);

    if (res && res->status == 200) {
        auto json_result = nlohmann::json::parse(res->body);
        common.movies = json_result["results"];

        if (json_result.contains("total_pages")) {
            common.total_pages = json_result["total_pages"];
        }

        if (!common.movies.empty()) {
            common.data_ready = true;

            // Create a directory for saving posters if it doesn't exist
            std::string poster_dir = "posters";
            if (!std::filesystem::exists(poster_dir) && !std::filesystem::create_directory(poster_dir)) {
                std::cerr << "Failed to create directory: " << poster_dir << std::endl;
                return;
            }

            std::vector<std::future<void>> download_tasks;

            // Download all posters asynchronously
            for (auto& movie : common.movies) {
                download_tasks.push_back(std::async(std::launch::async, &DownloadThread::DownloadPoster, this, std::ref(movie), std::ref(poster_dir), std::ref(common)));
            }

            // Wait for all downloads to complete
            for (auto& task : download_tasks) {
                task.get();
            }

            // Apply filtering in a synchronized manner
            {
                std::lock_guard<std::mutex> lock(mtx);
                movieService.FilterMovies();
            }
        }
    }
    else {
        std::cerr << "Failed to fetch movies.";
        if (res) {
            std::cerr << " Error code: " << res->status;
        }
        else {
            std::cerr << " (Request failed: nullptr response)";
        }
        std::cerr << std::endl;
    }
    common.data_ready = true;
}

// Updates the download URL (if needed in the future)
void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}
