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
    std::vector<Movie> movies;

    std::string api_key;
    std::vector<Movie> filtered_movies;
    std::string filter_query;
    std::unordered_map<std::string, Movie> movie_map;
    int page = 1;
    int total_pages = 1;
};