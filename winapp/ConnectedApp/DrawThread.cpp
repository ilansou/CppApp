#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

void DrawAppWindow(void* common_ptr) {
    auto common = (CommonObjects*)common_ptr;
    
}


void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}
