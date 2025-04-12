#include "imgui\imgui.h"
#include <Windows.h>
#include <fstream>
#include <shobjidl.h>
#include <string>

bool isLAASet = false;
std::string selectedFile = "";

bool GetBackup(const std::string& originalFilePath) {
    size_t lastSlashPos = originalFilePath.find_last_of("\\/");
    std::string directory = originalFilePath.substr(0, lastSlashPos + 1);
    std::string filename = originalFilePath.substr(lastSlashPos + 1);
    size_t extPos = filename.find_last_of('.');
    std::string baseName = filename.substr(0, extPos);
    std::string backupFilePath = directory + baseName + ".laabackup";

    if (CopyFileA(originalFilePath.c_str(), backupFilePath.c_str(), FALSE)) {
        return true;
    }
    else {
        return false;
    }
}

bool CheckAndSetLAA(const std::string& filePath, bool setLAA, bool& outIsLAA) {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) return false;

    IMAGE_DOS_HEADER dosHeader;
    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) return false;

    file.seekg(dosHeader.e_lfanew, std::ios::beg);
    DWORD ntSignature;
    file.read(reinterpret_cast<char*>(&ntSignature), sizeof(ntSignature));
    if (ntSignature != IMAGE_NT_SIGNATURE) return false;

    IMAGE_FILE_HEADER fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.Machine != IMAGE_FILE_MACHINE_I386) return false;

    outIsLAA = (fileHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE);

    if (setLAA) {
        GetBackup(filePath);
        file.seekp(-static_cast<int>(sizeof(fileHeader)), std::ios::cur);
        fileHeader.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;
        file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        outIsLAA = true;
    }

    file.close();
    return true;
}

std::string OpenFileDialog() {
    HRESULT hr;
    IFileDialog* pFileOpen;
    CoInitialize(NULL);
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

    if (SUCCEEDED(hr)) {
        COMDLG_FILTERSPEC rgSpec[] = {
            { L"Cool files (Executable)", L"*.exe" },
            { L"All Files", L"*.*" },
        };

        pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
        hr = pFileOpen->Show(NULL);

        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                std::wstring ws(pszFilePath);
                CoTaskMemFree(pszFilePath);
                pItem->Release();
                pFileOpen->Release();
                return std::string(ws.begin(), ws.end());
            }
        }
        pFileOpen->Release();
    }

    return "";
}

void CenterWindow(ImGuiViewport* viewport) {
    ImVec2 centerPos = ImVec2((viewport->WorkPos.x + viewport->WorkSize.x) / 2.0f,
        (viewport->WorkPos.y + viewport->WorkSize.y) / 2.0f);
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
}

void CreateCenterText(const char* Text, float Red = 1.0f, float Green = 1.0f, float Blue = 1.0f, float Transparency = 1.0f, float alignment = 0.5f) {
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(Text).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::TextColored(ImVec4(Red, Green, Blue, Transparency), Text);
}

bool CreateCenterButton(const char* Text, float Red = 1.0f, float Green = 1.0f, float Blue = 1.0f, float Transparency = 1.0f, float alignment = 0.5f) {
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(Text).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    ImVec4 color(Red, Green, Blue, Transparency);

    ImGui::PushStyleColor(ImGuiCol_Button, color);
    bool result = ImGui::Button(Text);
    ImGui::PopStyleColor();

    return result;
}

void ShowLAAImGuiWindow() {
    ImGui::GetIO().IniFilename = nullptr;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    CenterWindow(viewport);
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    CenterWindow(viewport);
    CreateCenterText("");
    CreateCenterText("Slap's LAA Patcher");
    CreateCenterText("This is the GitHub uploaded version");
    CreateCenterText("");
    CreateCenterText("Hi, this is a Miracle");
    CreateCenterText("With this app, you can get rid of too many errors");
    CreateCenterText("Such as Visual C++ Runtime errors, select a file first");
    CreateCenterText("Enjoy!");
    if (CreateCenterButton("Select a .exe File", 1.0f, 0.0f, 1.0f, 0.5f)) {
        selectedFile = OpenFileDialog();
        if (!selectedFile.empty()) {
            bool dummy = false;
            CheckAndSetLAA(selectedFile, false, isLAASet);
        }
    }

    if (!selectedFile.empty()) {
        size_t pos = selectedFile.find_last_of("/\\");
        std::string fileName = (pos != std::string::npos) ? selectedFile.substr(pos + 1) : selectedFile;
        std::string selectedLabel = "Currently selected file: " + fileName;
        std::string statusLabel = "LAA Status: " + std::string(isLAASet ? "Enabled" : "Disabled");
        CreateCenterText(selectedLabel.c_str(), 0.0f, 0.72f, 0.9f, 1.0f);
        CreateCenterText(statusLabel.c_str(), 0.0f, 0.72f, 0.9f, 1.0f);

        if (!isLAASet && CreateCenterButton("Enable LAA on the selected Executable", 0.0f, 1.0f, 0.0f, 0.5f)) {
            bool dummy;
            CheckAndSetLAA(selectedFile, true, dummy);
            isLAASet = true;
        }
    }

    ImGui::End();
}
