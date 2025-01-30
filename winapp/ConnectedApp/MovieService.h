#pragma once

#include "CommonObjects.h"
#include <vector>
#include <string>

class MovieService {
public:
    explicit MovieService(CommonObjects& commonObjects);

    void SaveMovieToFile(const Movie& movie, const std::string& filename = "movies.txt");
    void RemoveMovieFile(const Movie& movie, const std::string& filename = "movies.txt");
    void LoadMoviesFromFile(const std::string& filename = "movies.txt");
    void ClearMoviesFile(const std::string& filename = "movies.txt");
    void FilterMovies();
    void SortMoviesByTitle();
    void SortMoviesByVoteAverage();
    void SortMoviesByReleaseDate();

private:
    CommonObjects& commonObjects_;
    std::vector<Movie> LoadMoviesFromFileHelper(const std::string& filename);
};
