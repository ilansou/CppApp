#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

void DrawAppWindow(void* common_ptr) {
    auto common = (CommonObjects*)common_ptr;
    ImGui::Begin("Connected!");
    ImGui::Text("Trending Movies");

    if (common->data_ready) {
        // Set up the table with 5 columns
        if (ImGui::BeginTable("Movies", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            // Set column widths (adjust as needed)
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 170.0f);
            ImGui::TableSetupColumn("Overview", ImGuiTableColumnFlags_WidthFixed, 400.0f);
            ImGui::TableSetupColumn("Release Date", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Popularity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Vote Average", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableHeadersRow();

            // Add spacing between rows
            const float row_spacing = 10.0f;

            for (auto& movie : common->movies) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", movie.title.c_str()); // Ensure titles are visible

                ImGui::TableSetColumnIndex(1);
				if (!movie.overview.empty())
                    ImGui::TextWrapped("%s", movie.overview.c_str()); // Wrap overview text
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "No overview available."); // Display a message if no overview is available
               
                if (ImGui::IsItemHovered()) {
                    // Display the full overview in a tooltip when hovered
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f); // Limit tooltip width
                    ImGui::TextUnformatted(movie.overview.c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", movie.release_date.c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", movie.popularity);

                ImGui::TableSetColumnIndex(4);
				if (movie.vote_average > 5)
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f", movie.vote_average); // Highlight good ratings
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.2f", movie.vote_average); // Highlight bad ratings

                // Add spacing between rows
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + row_spacing);
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