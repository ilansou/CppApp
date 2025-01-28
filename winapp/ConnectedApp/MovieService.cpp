#include "MovieService.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iostream>

MovieService::MovieService(CommonObjects& commonObjects) : commonObjects_(commonObjects) {}

void MovieService::SaveMovieToFile(const Movie& movie, const std::string& filename) {
    // Load existing movies to check if the movie already exists
    std::ifstream infile(filename);
    std::vector<Movie> existing_movies;
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
            existing_movies.push_back({ title, overview, release_date, poster_path, popularity, vote_count, vote_average });
        }
        infile.close();
    }

    // Check if the movie already exists
    bool movie_exists = false;
    for (const auto& existing_movie : existing_movies) {
        if (existing_movie.title == movie.title) {
            movie_exists = true;
            break;
        }
    }

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
    std::ifstream infile(filename);
    std::vector<Movie> existing_movies;
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
            existing_movies.push_back({ title, overview, release_date, poster_path, popularity, vote_count, vote_average });
        }
        infile.close();
    }

    // Remove the movie from the list
    auto it = std::remove_if(existing_movies.begin(), existing_movies.end(), [&](const Movie& existing_movie) {
        return existing_movie.title == movie.title;
        });
    if (it != existing_movies.end()) {
        existing_movies.erase(it, existing_movies.end());
    }

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
    std::ifstream infile(filename);
    if (infile.is_open()) {
        commonObjects_.movies.clear();
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
            commonObjects_.movies.push_back({ title, overview, release_date, poster_path, popularity, vote_count, vote_average });
        }
        infile.close();
        FilterMovies(); // Filter movies after loading
    }
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
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

	for (const auto& movie : commonObjects_.movies) {
        std::string lower_title = movie.title;
        std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);

        std::string lower_overview = movie.overview;
        std::transform(lower_overview.begin(), lower_overview.end(), lower_overview.begin(), ::tolower);

        if (lower_title.find(lower_query) != std::string::npos ||
            lower_overview.find(lower_query) != std::string::npos) {
			commonObjects_.filtered_movies.push_back(movie);
        }
    }
}

void MovieService::SortMoviesByTitle() {
    std::sort(commonObjects_.filtered_movies.begin(), commonObjects_.filtered_movies.end(), [](const Movie& a, const Movie& b) {
        return a.title < b.title;
        });
}

void MovieService::SortMoviesByVoteAverage() {
    std::sort(commonObjects_.filtered_movies.begin(), commonObjects_.filtered_movies.end(), [](const Movie& a, const Movie& b) {
        return a.vote_average > b.vote_average; // Sort in descending order
        });
}

void MovieService::SortMoviesByReleaseDate() {
    std::sort(commonObjects_.filtered_movies.begin(), commonObjects_.filtered_movies.end(), [](const Movie& a, const Movie& b) {
        return a.release_date > b.release_date; // Sort in descending order (most recent first)
        });
}