#pragma once
#include <atomic>
#include <string>
#include <vector>

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

    /*std::string api_key;
    std::vector<Movie> filtered_movies;
    std::string filter_query;*/
};

