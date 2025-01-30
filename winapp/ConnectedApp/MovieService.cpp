#include "MovieService.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <ranges>
#include <iostream>

// Constructor for MovieService: takes a reference to CommonObjects and initializes it with the provided data
MovieService::MovieService(CommonObjects& commonObjects)
    : commonObjects_(commonObjects) {
}

// Helper function to load movies from a file
std::vector<Movie> MovieService::LoadMoviesFromFileHelper(const std::string& filename) {
    std::ifstream infile(filename);
    std::vector<Movie> movies;

    if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            std::stringstream ss(line);
            std::string title, overview, release_date, poster_path;
            double popularity, vote_average;
            int vote_count;
            std::getline(ss, title, '|');
            std::getline(ss, overview, '|');
            std::getline(ss, release_date, '|');
            std::getline(ss, poster_path, '|');
            ss >> popularity;
            ss.ignore();
            ss >> vote_count;
            ss.ignore();
            ss >> vote_average;
            movies.push_back({ title, overview, release_date, poster_path, popularity, vote_count, vote_average });
        }
        infile.close();
    }
    return movies;
}

void MovieService::SaveMovieToFile(const Movie& movie, const std::string& filename) {
    // Load existing movies to check if the movie already exists
    auto existing_movies = LoadMoviesFromFileHelper(filename);

    // Check if the movie already exists using std::ranges::any_of
    bool movie_exists = std::ranges::any_of(existing_movies, [&](const Movie& existing_movie) {
        return existing_movie.title == movie.title;
        });

    // If the movie does not exist, save it to the file
    if (!movie_exists) {
        std::ofstream outfile(filename, std::ios::app);
        if (outfile.is_open()) {
            outfile << movie.title << "|" << movie.overview << "|"
                << movie.release_date << "|" << movie.poster_path << "|"
                << movie.popularity << "|" << movie.vote_count << "|"
                << movie.vote_average << "\n";
            outfile.close();
            std::cout << "Saved movie: " << movie.title << std::endl;
        }
        else {
            std::cerr << "Failed to save movie: " << movie.title << std::endl;
        }
    }
}

void MovieService::RemoveMovieFile(const Movie& movie, const std::string& filename) {
    // Load existing movies
    auto existing_movies = LoadMoviesFromFileHelper(filename);

    // Remove the movie from the list using std::erase_if (C++20)
    std::erase_if(existing_movies, [&](const Movie& existing_movie) {
        return existing_movie.title == movie.title;
        });

    // Save the updated list back to the file
    std::ofstream outfile(filename, std::ios::trunc);
    if (outfile.is_open()) {
        for (const auto& existing_movie : existing_movies) {
            outfile << existing_movie.title << "|" << existing_movie.overview << "|"
                << existing_movie.release_date << "|" << existing_movie.poster_path << "|"
                << existing_movie.popularity << "|" << existing_movie.vote_count << "|"
                << existing_movie.vote_average << "\n";
        }
        outfile.close();
        std::cout << "Removed movie: " << movie.title << std::endl;
    }
    else {
        std::cerr << "Failed to remove movie: " << movie.title << std::endl;
    }
}

void MovieService::LoadMoviesFromFile(const std::string& filename) {
    commonObjects_.movies = LoadMoviesFromFileHelper(filename);
    FilterMovies(); // Filter movies after loading
}

void MovieService::ClearMoviesFile(const std::string& filename) {
    std::ofstream outfile(filename, std::ios::trunc); // Open in truncation mode to clear the file
    if (outfile.is_open()) {
        outfile.close();
        std::cout << "Cleared movies file: " << filename << std::endl;
    }
    else {
        std::cerr << "Failed to clear movies file: " << filename << std::endl;
    }
}

void MovieService::FilterMovies() {
    commonObjects_.filtered_movies.clear();
    std::string lower_query = commonObjects_.filter_query;
    std::ranges::transform(lower_query, lower_query.begin(), ::tolower); // Convert the query to lowercase

    // Use std::ranges::copy_if to filter movies
    std::ranges::copy_if(commonObjects_.movies, std::back_inserter(commonObjects_.filtered_movies),
        [&](const Movie& movie) {
            std::string lower_title = movie.title;
            std::ranges::transform(lower_title, lower_title.begin(), ::tolower);

            std::string lower_overview = movie.overview;
            std::ranges::transform(lower_overview, lower_overview.begin(), ::tolower);

            return lower_title.find(lower_query) != std::string::npos ||
                lower_overview.find(lower_query) != std::string::npos;
        });
}

void MovieService::SortMoviesByTitle() {
    std::ranges::sort(commonObjects_.filtered_movies, [](const Movie& a, const Movie& b) {
        return a.title < b.title;
        });
}

void MovieService::SortMoviesByVoteAverage() {
    std::ranges::sort(commonObjects_.filtered_movies, [](const Movie& a, const Movie& b) {
        return a.vote_average > b.vote_average; // Sort in descending order
        });
}

void MovieService::SortMoviesByReleaseDate() {
    std::ranges::sort(commonObjects_.filtered_movies, [](const Movie& a, const Movie& b) {
        return a.release_date > b.release_date; // Sort in descending order (most recent first)
        });
}