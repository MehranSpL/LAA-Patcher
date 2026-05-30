#include "laa_tool.h"

std::string selectedFile;
bool isLAASet = false;

float g_StatusFade = 0.0f;
std::string g_StatusMessage = "No executable selected, select one first to check the status";
bool g_StatusSuccess = false;

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



void ShowLAAImGuiWindow()
{
    ImGuiIO& io = ImGui::GetIO();

    g_StatusFade += io.DeltaTime * 4.0f;
    if (g_StatusFade > 1.0f)
        g_StatusFade = 1.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin(
        "Main",
        nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    ImGui::Text("\nSlap's LAA Patcher");

    ImGui::SameLine(
        ImGui::GetWindowWidth() - 50);

    if (ImGui::Button("X"))
    {
        PostQuitMessage(0);
    }

    ImGui::Separator();

    ImGui::BeginChild("FileCard",
        ImVec2(0, 110),
        true);

    ImGui::Text("Executable:");

    if (selectedFile.empty())
    {
        ImGui::TextDisabled(
            "No executable selected");
    }
    else
    {
        size_t pos =
            selectedFile.find_last_of("/\\");

        std::string name =
            selectedFile.substr(pos + 1);

        ImGui::TextWrapped("%s",
            name.c_str());
    }

    ImGui::Spacing();

    if (ImGui::Button(
        "Browse for Executable",
        ImVec2(-1, 40)))
    {
        selectedFile = OpenFileDialog();

        if (!selectedFile.empty())
        {
            CheckAndSetLAA(
                selectedFile,
                false,
                isLAASet);

            g_StatusFade = 0.0f;

            g_StatusSuccess = isLAASet;

            g_StatusMessage =
                isLAASet ?
                "Large Address Aware is enabled"
                :
                "Large Address Aware is disabled";
        }
    }

    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild(
        "StatusCard",
        ImVec2(0, 90),
        true);

    ImVec4 color =
        g_StatusSuccess ?
        ImVec4(0.2f, 1.0f, 0.3f, g_StatusFade)
        :
        ImVec4(1.0f, 0.35f, 0.35f, g_StatusFade);

    ImGui::TextColored(
        color,
        "%s",
        g_StatusSuccess ?
        "[+] | Enabled "
        :
        "[-] | Disabled");

    ImGui::Spacing();

    ImGui::TextColored(
        ImVec4(1, 1, 1, g_StatusFade),
        "%s",
        g_StatusMessage.c_str());

    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild(
        "ActionCard",
        ImVec2(0, 90),
        true);

    ImGui::Text("Actions:");

    ImGui::Spacing();

    if (!selectedFile.empty())
    {
        if (!isLAASet)
        {
            if (ImGui::Button(
                "Enable Large Address Aware",
                ImVec2(-1, 40)))
            {
                bool dummy;

                CheckAndSetLAA(
                    selectedFile,
                    true,
                    dummy);

                isLAASet = true;

                g_StatusFade = 0.0f;

                g_StatusSuccess = true;

                g_StatusMessage =
                    "Successfully enabled LAA on selected executable!";
            }
        }
        else
        {
            ImGui::BeginDisabled();

            ImGui::Button(
                "Already Enabled",
                ImVec2(-1, 40));

            ImGui::EndDisabled();
        }
    }

    else {
        ImGui::TextDisabled("Select something first dude");
    }

    ImGui::EndChild();

    ImGui::End();
}