#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm> // for std::sort
#include <cctype>    // for std::tolower

struct Movie {
    std::string title;
    std::string overview;
    std::string release_date;
    std::string poster_path;
    double popularity;
    int vote_count;
    double vote_average;
};

struct CommonObjects {
    std::atomic_bool exit_flag = false;
    std::atomic_bool start_download = false;
    std::atomic_bool data_ready = false;
    std::string url;
    std::vector<Movie> movies;

    std::string api_key;
    std::vector<Movie> filtered_movies;
    std::string filter_query;
    std::unordered_map<std::string, Movie> movie_map;
    
    void FilterMovies() {
        filtered_movies.clear();
        std::string lower_query = filter_query;
        std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

        for (const auto& movie : movies) {
            std::string lower_title = movie.title;
            std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);

            std::string lower_overview = movie.overview;
            std::transform(lower_overview.begin(), lower_overview.end(), lower_overview.begin(), ::tolower);

            if (lower_title.find(lower_query) != std::string::npos ||
                lower_overview.find(lower_query) != std::string::npos) {
                filtered_movies.push_back(movie);
            }
        }
    }

    void SortMoviesByTitle() {
        std::sort(filtered_movies.begin(), filtered_movies.end(), [](const Movie& a, const Movie& b) {
            return a.title < b.title;
            });
    }

    void SortMoviesByVoteAverage() {
        std::sort(filtered_movies.begin(), filtered_movies.end(), [](const Movie& a, const Movie& b) {
            return a.vote_average > b.vote_average; // Sort in descending order
            });
    }

    void SortMoviesByReleaseDate() {
        std::sort(filtered_movies.begin(), filtered_movies.end(), [](const Movie& a, const Movie& b) {
            return a.release_date > b.release_date; // Sort in descending order (most recent first)
            });
    }
};