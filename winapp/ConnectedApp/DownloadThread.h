#pragma once
#include "CommonObjects.h"
#include "MovieService.h"
#include <string>
#include <future>
#include <filesystem>

class DownloadThread {
public:
    void operator()(CommonObjects& common);
    void SetUrl(std::string_view new_url);

private:
    std::string _download_url;
    std::string GetPosterFilename(const std::string& poster_path);
    void DownloadPoster(Movie& movie, const std::string& poster_dir, CommonObjects& common);
};
