#pragma once
#include "DrawThread.h"
#include "GuiMain.h"
#include "MovieService.h"  
#include "DownloadThread.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <mutex>
#include "../../shared/ImGuiSrc/imgui_internal.h"

extern std::mutex mtx;

// Global texture cache to avoid loading the same texture multiple times
std::unordered_map<std::string, ID3D11ShaderResourceView*> texture_cache;

void DrawLoadingDots() {
    double time = ImGui::GetTime();
    int num_dots = (int)(time * 3.0f) % 4; // Cycle through 0-3 dots
    std::string dots(num_dots, '.');
    ImGui::Text("Loading Poster%s", dots.c_str());
}



void DrawAppWindow(void* common_ptr) {
    MovieService movieService(*(CommonObjects*)common_ptr);
    auto common = static_cast<CommonObjects*>(common_ptr);
    static DownloadThread downloadThread;

    ImGui::Begin("Connected!");

    // Get the window width to center elements
    float windowWidth = ImGui::GetContentRegionAvail().x;

    // Add spacing above the title
    ImGui::Spacing();
    ImGui::Spacing();

    // Center the title
    float textWidth = ImGui::CalcTextSize("Trending Movies").x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

    // Increase font size
    ImGui::SetWindowFontScale(1.8f);

    // Styled & Colored Title
    ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.0f, 1.0f), "Trending Movies");  // Orange color for a cinematic feel

    // Reset font scale to default
    ImGui::SetWindowFontScale(1.0f);

    // Add spacing below the title
    ImGui::Spacing();
    ImGui::Separator();  // Adds a horizontal line for a sleek look
    ImGui::Spacing();


    static bool loading = false;


    // Centering the Search Bar
    float searchBarWidth = 450.0f;  // Set fixed width for input box
    ImGui::SetCursorPosX((windowWidth - searchBarWidth) * 0.5f);  // Center it

    // Search Input Box (Styled)
    ImGui::PushItemWidth(searchBarWidth);
    char filter_buffer[256];
    strncpy_s(filter_buffer, common->filter_query.c_str(), sizeof(filter_buffer));
    filter_buffer[sizeof(filter_buffer) - 1] = '\0';

    if (ImGui::InputText("Search", filter_buffer, sizeof(filter_buffer))) {
        common->filter_query = filter_buffer;
        movieService.FilterMovies();
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();  // Add spacing before buttons

    // Sorting Buttons (Centered)
    float buttonWidth = 100.0f;
    float totalSortWidth = (buttonWidth * 3) + 40;  // Adjusted for three buttons with spacing
    ImGui::SetCursorPosX((windowWidth - totalSortWidth) * 0.5f);  // Center sort buttons

    if (ImGui::Button("Sort by Title", ImVec2(buttonWidth, 25))) {
        movieService.SortMoviesByTitle();
    }
    ImGui::SameLine();
    if (ImGui::Button("Sort by Rating", ImVec2(buttonWidth, 25))) {
        movieService.SortMoviesByVoteAverage();
    }
    ImGui::SameLine();
    if (ImGui::Button("Sort by Date", ImVec2(buttonWidth, 25))) {
        movieService.SortMoviesByReleaseDate();
    }

    ImGui::Spacing();  // Add spacing before next row

    // Load and Clear Buttons (Centered)
    float totalLoadWidth = (buttonWidth * 2) + 20;  // Two buttons with spacing
    ImGui::SetCursorPosX((windowWidth - totalLoadWidth) * 0.5f);  // Center them

    if (ImGui::Button("Load Movies", ImVec2(buttonWidth, 25))) {
        movieService.LoadMoviesFromFile();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Movies", ImVec2(buttonWidth, 25))) {
        movieService.ClearMoviesFile();
    }


    // Pagination Section
    ImGui::Spacing();
    ImVec2 buttonSize(80, 25);

    // Get available width for centering
    float totalWidth = (buttonSize.x * 2) + 100; // Buttons + spacing
    float centerPosX = (windowWidth - totalWidth) * 0.5f;


    // Set height for pagination block
    float paginationHeight = 50;  // Ensures all elements are aligned
    ImGui::Dummy(ImVec2(0, paginationHeight)); // Create space for alignment

    // Move cursor to align everything
    ImGui::SetCursorPosX(centerPosX);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    // Left Button (Previous)
    if (ImGui::Button("Previous", buttonSize) && common->page > 1 && !loading) {
        common->page--;
        common->data_ready = false;
        loading = true;
        std::thread([](CommonObjects* c) {
            DownloadThread downloadThread;
            downloadThread(*c);
            c->data_ready = true;
            loading = false;
            }, common).detach();
    }

    // Space between elements
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0));
    ImGui::SameLine();

    // Centered "Page: X" with increased font size
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Page: %d", common->page);
    ImGui::SetWindowFontScale(1.0f);

    // Space before Next button
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0));
    ImGui::SameLine();

    // Right Button (Next)
    if (ImGui::Button("Next", buttonSize) && !loading) {
        common->page++;
        common->data_ready = false;
        loading = true;
        std::thread([](CommonObjects* c) {
            DownloadThread downloadThread;
            downloadThread(*c);
            c->data_ready = true;
            loading = false;
            }, common).detach();
    }

    if (loading) {
        ImGui::OpenPopup("LoadingPopup");

        if (ImGui::BeginPopupModal("LoadingPopup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
            double time = ImGui::GetTime();
            int num_dots = (int)(time * 3.0f) % 4; // Cycle through 0-3 dots
            std::string dots(num_dots, '.');
            ImGui::Text("Loading Movies%s", dots.c_str());
            ImGui::EndPopup();
        }
    }



    //ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // Movie Table
    if (common->data_ready) {
        ImGui::Spacing();
        if (ImGui::BeginTable("Movies", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Poster", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 170.0f);
            ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed, 400.0f);
            ImGui::TableSetupColumn("Release Date", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Popularity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Vote Average", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableHeadersRow();

            std::lock_guard<std::mutex> lock(mtx);
            for (auto& movie : common->filtered_movies) {
                ImGui::TableNextRow();

                // Poster Column
                ImGui::TableSetColumnIndex(0);
                if (!movie.poster_path.empty()) {
                    std::string local_poster_path = movie.poster_path;
                    if (!std::filesystem::exists(local_poster_path)) {
                        DrawLoadingDots();
                    }
                    else {
                        if (texture_cache.find(local_poster_path) == texture_cache.end()) {
                            int width = 0, height = 0;
                            ID3D11ShaderResourceView* texture = nullptr;
                            if (LoadTextureFromFile(local_poster_path.c_str(), &texture, &width, &height)) {
                                texture_cache[local_poster_path] = texture;
                            }
                        }
                        if (texture_cache.find(local_poster_path) != texture_cache.end()) {
                            ImGui::Image((void*)texture_cache[local_poster_path], ImVec2(100, 150));
                        }
                    }
                }
                else {
                    DrawLoadingDots();
                }

                // Title Column
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", movie.title.c_str());

                // Overview Column
                ImGui::TableSetColumnIndex(2);
                ImGui::TextWrapped("%s", movie.overview.empty() ? "No overview available." : movie.overview.c_str());

                // Release Date Column
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", movie.release_date.c_str());

                // Vote Average Column
                ImGui::TableSetColumnIndex(5);
                ImGui::TextColored(movie.vote_average > 5 ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "%.2f", movie.vote_average);
            }
            ImGui::EndTable();

            // "Total Pages"
            ImGui::Text("Total Pages: %d", common->total_pages);
        }
    }
    ImGui::End();
}

void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}
