#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Recipe, name, cuisine, difficulty, cookTimeMinutes ,image)

void DownloadThread::operator()(CommonObjects& common)
{

}

void DownloadThread::SetUrl(std::string_view new_url)
{

}
