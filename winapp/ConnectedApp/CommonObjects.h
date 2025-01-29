#pragma once

#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <unordered_map>

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

    std::vector<Movie> movies; // Vector to store all fetched movies.

    std::string api_key;

    std::vector<Movie> filtered_movies; // Vector to store the filtered list of movies.
    std::string filter_query; // String to store the current filter query.

    std::unordered_map<std::string, Movie> movie_map; // Map to store movies, using the movie title as the key for fast lookups.
};