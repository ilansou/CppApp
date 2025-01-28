#pragma once
#include "DrawThread.h"
#include "GuiMain.h"
#include "MovieService.h"  
#include "../../shared/ImGuiSrc/imgui.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <mutex>

extern std::mutex mtx;

// Global texture cache to avoid loading the same texture multiple times
std::unordered_map<std::string, ID3D11ShaderResourceView*> texture_cache;

void DrawAppWindow(void* common_ptr) {
	MovieService movieService(*(CommonObjects*)common_ptr);
    auto common = static_cast<CommonObjects*>(common_ptr);
    ImGui::Begin("Connected!");
    ImGui::Text("Trending Movies");

    // Filter input
    char filter_buffer[256];
    strncpy_s(filter_buffer, common->filter_query.c_str(), sizeof(filter_buffer));
    filter_buffer[sizeof(filter_buffer) - 1] = '\0';
    if (ImGui::InputText("Filter", filter_buffer, sizeof(filter_buffer))) {
        common->filter_query = filter_buffer;
		movieService.FilterMovies();
    }

    // Sorting buttons
    if (ImGui::Button("Sort by Title")) {
		movieService.SortMoviesByTitle();
    }
    ImGui::SameLine();
    if (ImGui::Button("Sort by Vote Average")) {
		movieService.SortMoviesByVoteAverage();
    }
    ImGui::SameLine();
    if (ImGui::Button("Sort by Release Date")) {
		movieService.SortMoviesByReleaseDate();
    }

    // Load and Clear buttons
    if (ImGui::Button("Load Movies")) {
		movieService.LoadMoviesFromFile();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Movies")) {
		movieService.ClearMoviesFile();
    }

    if (common->data_ready) {
        // Set up the table with 6 columns (added poster column)
        if (ImGui::BeginTable("Movies", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            // Set column widths (adjust as needed)
            ImGui::TableSetupColumn("Poster", ImGuiTableColumnFlags_WidthFixed, 200.0f); // Poster column
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 170.0f);
            ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed, 400.0f);
            ImGui::TableSetupColumn("Release Date", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Popularity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Vote Average", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableHeadersRow();

            {
				std::lock_guard<std::mutex> lock(mtx);
                for (auto& movie : common->filtered_movies) {
                    ImGui::TableNextRow();

                    // Poster column
                    ImGui::TableSetColumnIndex(0);
                    if (!movie.poster_path.empty()) {
                        // Construct the full local path to the poster
                        std::string local_poster_path = movie.poster_path;

                        // Check if the file exists
                        if (!std::filesystem::exists(local_poster_path)) {
                            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Poster file not found.");
                        }
                        else {
                            // Check if the texture is already cached
                            if (texture_cache.find(local_poster_path) == texture_cache.end()) {
                                // Load the texture if it's not in the cache
                                int my_image_width = 0;
                                int my_image_height = 0;
                                ID3D11ShaderResourceView* my_texture = nullptr;
                                bool ret = LoadTextureFromFile(local_poster_path.c_str(), &my_texture, &my_image_width, &my_image_height);
                                if (ret) {
                                    // Cache the texture

                                    texture_cache[local_poster_path] = my_texture;
                                }
                                else {
                                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load poster.");
                                }
                            }

                            // Display the cached texture
                            if (texture_cache.find(local_poster_path) != texture_cache.end()) {
                                ID3D11ShaderResourceView* my_texture = texture_cache[local_poster_path];
                                ImGui::Image((void*)my_texture, ImVec2(100, 150)); // Adjust size as needed
                            }
                        }
                    }
                    else {
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No poster available.");
                    }

                    // Title column
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", movie.title.c_str());

                    // Overview column
                    ImGui::TableSetColumnIndex(2);
                    if (!movie.overview.empty())
                        ImGui::TextWrapped("%s", movie.overview.c_str());
                    else
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No overview available.");

                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(movie.overview.c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }

                    // Release Date column
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", movie.release_date.c_str());

                    // Popularity column
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%.2f", movie.popularity);

                    // Vote Average column
                    ImGui::TableSetColumnIndex(5);
                    if (movie.vote_average > 5)
                        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f", movie.vote_average);
                    else
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.2f", movie.vote_average);

                    // Save column
                    ImGui::TableSetColumnIndex(6);
                    ImGui::PushID(movie.title.c_str()); // Use movie title as a base for the ID

                    if (ImGui::Button("Save")) {
						movieService.SaveMovieToFile(movie);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove")) {
						movieService.RemoveMovieFile(movie);
                    }

                    ImGui::PopID(); // Important: Pop the ID
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}