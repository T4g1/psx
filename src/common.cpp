#include "common.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


bool in_range(uint32_t address, uint32_t start, size_t size)
{
    return start <= address && address < start + size;
}


uint32_t extract(uint32_t data, size_t from, size_t size)
{
    uint32_t mask = (1 << size) - 1;

    return (data >> from) & mask;
}


void ToggleButton(const char *text, bool *boolean)
{
    ImVec4 active = ImVec4(0.0f, 1.0f, 0.0f, 0.3f);
    ImVec4 inactive = ImVec4(1.0f, 0.0f, 0.0f, 0.3f);

    if (*boolean) {
        ImGui::PushStyleColor(ImGuiCol_Button, active);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, inactive);
    }

    if (ImGui::Button(text)) {
        *boolean = !*boolean;
    }

    ImGui::PopStyleColor(1);
}
