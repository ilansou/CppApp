#include "DownloadThread.h"
#include "MovieService.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <ranges>

extern std::mutex mtx; // External mutex for synchronizing access to shared resources

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Movie, title, overview, release_date, poster_path, popularity, vote_count, vote_average)

// Lambda function to get the API key from environment variables
auto GetApiKeyFromEnv = []() -> std::string {
    char* api_key = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&api_key, &len, "TMDB_API_KEY"); // Get the API key from the environment variable. _dupenv_s is a secure version for windows.
    if (err || api_key == nullptr) {
        std::cerr << "Environment variable TMDB_API_KEY is not set." << std::endl;
        return "";
    }
    std::string api_key_str(api_key);
    free(api_key);
    return api_key_str;
    };

// Lambda function to get the poster filename from the poster path
auto GetPosterFilename = [](const std::string& poster_path) -> std::string {
    size_t lastSlash = poster_path.find_last_of('/');
    return (lastSlash == std::string::npos) ? poster_path : poster_path.substr(lastSlash + 1);
    };

// Operator overload to run the download thread
void DownloadThread::operator()(CommonObjects& common) {
    MovieService movieService(common);

    // Create an HTTP client to interact with the TMDb API
    httplib::Client cli("api.themoviedb.org");
    const std::string API_KEY = GetApiKeyFromEnv();

    // Set headers for TMDb API
    httplib::Headers headers = {
        {"Authorization", "Bearer " + API_KEY},
        {"Accept", "application/json"}
    };

    // Make a GET request to fetch trending movies
    auto res = cli.Get("/3/trending/movie/week?api_key=" + API_KEY, headers);

    if (res && res->status == 200) {
        // Parse the JSON response
        auto json_result = nlohmann::json::parse(res->body);
        common.movies = json_result["results"];

		if (!common.movies.empty()) { // If movies are fetched successfully
            common.data_ready = true;

            // Create a directory to save posters
            std::string poster_dir = "posters";
            if (!std::filesystem::exists(poster_dir)) {
                std::filesystem::create_directory(poster_dir);
            }

			// Create an HTTP client to download images
            httplib::Client img_cli("https://image.tmdb.org");

            // Download and save posters
            for (auto& movie : common.movies) {
                if (!movie.poster_path.empty()) {
                    std::string poster_url = "/t/p/w500" + movie.poster_path;
                    std::string local_path = poster_dir + "/" + GetPosterFilename(movie.poster_path);

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
				// Start of a critical section protected by the mutex 'mtx'. This block ensures that only one thread can access and modify the shared resource common.movie_map at a time.
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    common.movie_map[movie.title] = movie;
                }// The lock_guard automatically releases the lock when it goes out of scope.

                movieService.FilterMovies();
            }
        }
    }
    else {
        std::cerr << "Failed to fetch movies. Error code: " << res->status << std::endl;
    }
}

// Function to set the URL for the download thread
void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}
