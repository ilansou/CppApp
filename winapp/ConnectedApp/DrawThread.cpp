#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

void DrawAppWindow(void* common_ptr) {
    auto common = (CommonObjects*)common_ptr;
    ImGui::Begin("Connected!");
    ImGui::Text("Trending Movies");

    if (common->data_ready) {
        if (ImGui::BeginTable("Movies", 5)) {
            ImGui::TableSetupColumn("Title");
            ImGui::TableSetupColumn("Overview");
            ImGui::TableSetupColumn("Release Date");
            ImGui::TableSetupColumn("Popularity");
            ImGui::TableSetupColumn("Vote Average");
            ImGui::TableHeadersRow();

            for (auto& movie : common->movies) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(movie.title.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(movie.overview.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(movie.release_date.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", movie.popularity);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.1f", movie.vote_average);
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
