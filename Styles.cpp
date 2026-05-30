#include "Styles.h"

void SetupStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding =
        ImVec2(20, 20);

    style.ItemSpacing =
        ImVec2(12, 12);

    style.FramePadding =
        ImVec2(12, 10);

    style.WindowRounding = 20.0f;
    style.ChildRounding = 14.0f;
    style.FrameRounding = 12.0f;
    style.PopupRounding = 12.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 12.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;

    auto& c = style.Colors;

    c[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.10f, 0.11f, 1.0f);

    c[ImGuiCol_Button] = ImVec4(0.17f, 0.44f, 1.00f, 1.0f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.53f, 1.00f, 1.0f);
    c[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.37f, 0.95f, 1.0f);

    c[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.16f, 1.0f);

    c[ImGuiCol_Header] = ImVec4(0.17f, 0.44f, 1.00f, 0.6f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.53f, 1.00f, 0.8f);
}