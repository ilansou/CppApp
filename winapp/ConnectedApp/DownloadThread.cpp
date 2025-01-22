#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Movie, title, overview, release_date, poster_path, popularity, vote_count, vote_average)

void DownloadThread::operator()(CommonObjects& common) {
    httplib::Client cli("api.themoviedb.org");

    const std::string API_KEY = "fd05a4fdf85e914ec224c2016dc73bd2";

    // Set headers for TMDb API
    httplib::Headers headers = {
		{"Authorization", "Bearer " + API_KEY},
        {"Accept", "application/json"}
    };

    auto res = cli.Get("/3/trending/movie/week?api_key=" + API_KEY, headers);

    if (res && res->status == 200) {
        auto json_result = nlohmann::json::parse(res->body);
        common.movies = json_result["results"];
        ///common.filtered_movies = common.movies; 
        if (!common.movies.empty())
            common.data_ready = true;
    }
}


void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}
