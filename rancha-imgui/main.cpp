﻿// Dear ImGui: standalone example application for DirectX 12
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important: to compile on 32-bit systems, the DirectX12 backend requires code to be compiled with '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// This define is set in the example .vcxproj file and need to be replicated in your app or by adding it to your imconfig.h file.

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include "rancha.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

//Global Account and Charlist variables
std::vector<Account> acctList{ Account(L"Blank", L"Blank") };
std::vector<Character> charList{ Character(L"Blank", 1, 1) };
std::vector<Team> teamList{ Team() };
//Used to reference the account associated with a character in charList
//The acctListIndex will allign with charListIndex
std::vector<int> acctListIndex{ 0 };

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device*                g_pd3dDevice = NULL;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
static ID3D12Fence*                 g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3*             g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("rancha-imgui"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool rancha_active = true;
    bool loginWin = true;
    bool acctWin = false;
    bool charWin = false;
    bool teamWin = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
     
        static float f = 0.0f;
        static int counter = 0;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::Begin("rancha-imgui", &rancha_active, ImGuiWindowFlags_MenuBar);                          // Create a window called "Hello, world!" and append into it.

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Login")) { teamWin = false;  acctWin = false;  charWin = false;  loginWin = true; }
                if (ImGui::MenuItem("Accounts")) { teamWin = false;  loginWin = false;  charWin = false;  acctWin = true; }
                if (ImGui::MenuItem("Characters")) { teamWin = false;  loginWin = false;  acctWin = false; charWin = true; }
                if (ImGui::MenuItem("Teams")) { loginWin = false;  acctWin = false; charWin = false; teamWin = true;}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        if (loginWin)
        {
            ImGui::BeginChild("Login Input");
            static char password[64] = "";
            static char acctname[64] = "";
            static char charname[64] = "";
            ImGui::Text("Login");
            ImGui::Text("Account:");
            ImGui::SameLine();
            ImGui::InputText("##account", acctname, IM_ARRAYSIZE(acctname));
            ImGui::Text("Password:");
            ImGui::SameLine();
            ImGui::InputText("##password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
            ImGui::Text("Character:");
            ImGui::SameLine();
            ImGui::InputText("##character", charname, IM_ARRAYSIZE(charname));
            const char* realms[] = { "Albion", "Midgard", "Hibernia"};
            static int realm_current_idx = 0; // Here we store our selection data as an index.
            ImGui::Text("Realm");
            if (ImGui::BeginListBox("##realmlist", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 3)))
            {
                for (int n = 0; n < IM_ARRAYSIZE(realms); n++)
                {
                    const bool realm_selected = (realm_current_idx == n);
                    if (ImGui::Selectable(realms[n], realm_selected))
                        realm_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (realm_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
            ImGui::Text("Server");
            const char* servers[] = { "Ywain1", "Ywain2", "Ywain3", "Ywain4", "Ywain5", "Ywain6", "Ywain7", "Ywain8", "Ywain9", "Ywain10", "Gaheris"};
            static int server_current_idx = 0;
            if (ImGui::BeginListBox("##serverlist", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 11)))
            {
                for (int n = 0; n < IM_ARRAYSIZE(servers); n++)
                {
                    const bool is_selected = (server_current_idx == n);
                    if (ImGui::Selectable(servers[n], is_selected))
                        server_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            static int loginClicked = 0;
            if (ImGui::Button("Login"))
                loginClicked++;
            if (loginClicked & 1)
            {
                //account only
                if (strlen(charname) == 0) {
                    LaunchDaoc(server_current_idx, charToWChar(acctname), charToWChar(password), NULL, NULL);
                }
                else {
                    LaunchDaoc(server_current_idx, charToWChar(acctname), charToWChar(password), charToWChar(charname), realm_current_idx);
                }
                loginClicked++;
            }
            ImGui::SameLine();
            static int saveAcctClicked = 0;
            if (ImGui::Button("Save Account"))
                saveAcctClicked++;
            if (saveAcctClicked & 1)
            {
                //account only
                Account tmpacct = Account{ charToWChar(acctname), charToWChar(password) };
                SaveAcct(tmpacct);
                saveAcctClicked++;
            }
            ImGui::SameLine();
            static int saveCharClicked = 0;
            if (ImGui::Button("Save Character"))
                saveCharClicked++;
            if (saveCharClicked & 1)
            {
                Account tmpacct = Account{ charToWChar(acctname), charToWChar(password) };
                Character tmpchar = Character{ charToWChar(charname), server_current_idx , realm_current_idx };
                SaveChar(tmpacct, tmpchar);
                saveCharClicked++;
            }
            ImGui::EndChild();
        }

        if (acctWin) {

            ImGui::BeginChild("##acctscreen");
            //First clear the character and account list
            charList.clear();
            acctList.clear();

            //Populate the account list
            FetchAccounts(acctList);

            //For each account draw a list of columns
            //Count the number of characters in acctList https://stackoverflow.com/questions/22269435/how-to-iterate-through-a-list-of-objects-in-c
            //Keep an index to store for later lookup
            int charCount = 0;
            int acctIndex = 0;
            int charIndex = 0;

            bool firstPass = true;

            static int acctSelected;
            static int server_current_idx = -1;
            //Bool flag to set current_item and server_current_inx to 10 at first;
                
            static int acctLoginClicked = 0;
            if (ImGui::Button("Login"))
                acctLoginClicked++;
            if (acctLoginClicked & 1)
            {
                LaunchDaoc(server_current_idx, WstringToWchar(acctList[acctSelected].GetAcctName()), WstringToWchar(acctList[acctSelected].GetAcctPassword()), NULL, NULL);
                acctLoginClicked++;
            }
            ImGui::SameLine();
            ImGui::Text("Server");
            ImGui::SameLine();
            const char* servers[] = { "Ywain1", "Ywain2", "Ywain3", "Ywain4", "Ywain5", "Ywain6", "Ywain7", "Ywain8", "Ywain9", "Ywain10", "Gaheris" };
            static const char* current_item = NULL;
            if (server_current_idx == -1) {
                current_item = servers[10];
                server_current_idx = 10;
            }
            if (ImGui::BeginCombo("##acctServerlist", servers[server_current_idx], ImGuiComboFlags_PopupAlignLeft))
            {
                for (int n = 0; n < IM_ARRAYSIZE(servers); n++)
                {

                        const bool is_selected = (server_current_idx == n);
                        if (ImGui::Selectable(servers[n], is_selected))
                            server_current_idx = n;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }



            ImGui::Text("Account:");
            //For each character in each account
            for (auto& acct : acctList) {
                //if there are characters
                char buf[32];
                sprintf_s(buf, "%s", wstring_to_utf8(acct.GetAcctName()).c_str());
                if (ImGui::Selectable(buf, acctSelected == acctIndex))
                    acctSelected = acctIndex;
                //ImGui::Text(WstringToChar(acct.GetAcctName()));
                acctIndex++;
            }

            ImGui::EndChild();
        }

        if (charWin) {
            ImGui::BeginChild("##charscreen");
            //First clear the character and account list
            charList.clear();
            acctList.clear();

            //Populate the account list
            FetchAccounts(acctList);

            //For each account draw a list of columns
            //Count the number of characters in acctList https://stackoverflow.com/questions/22269435/how-to-iterate-through-a-list-of-objects-in-c
            //Keep an index to store for later lookup
            int charCount = 0;
            int acctIndex = 0;
            int charIndex = 0;

            static int charSelected;
            static int acctSelected;
                
                
            static int acctLoginClicked = 0;
            if (ImGui::Button("Login"))
                acctLoginClicked++;
            if (acctLoginClicked & 1)
            {
                LaunchDaoc(acctList[acctSelected].GetChars()[charIndex].GetServer(), WstringToWchar(acctList[acctSelected].GetAcctName()),
                    WstringToWchar(acctList[acctSelected].GetAcctPassword()), WstringToWchar(acctList[acctSelected].GetChars()[charIndex].GetName()),
                    acctList[acctSelected].GetChars()[charIndex].GetRealm());
                acctLoginClicked++;
            }

            
            int selectedIndex = 0;
            //For each character in each account
            for (auto& acct : acctList) {
                //Reset charIndex for each account
                charIndex = 0;
                //if there are characters
                //ImGui::Text("Account:");
                //ImGui::SameLine();
                static int selected = -1;
                
                ImGui::Text(wstring_to_utf8(acct.GetAcctName()).c_str());
                if (acct.GetChars().size() > 0) {
                    ImGui::Spacing();
                    if (ImGui::BeginTable(wstring_to_utf8(acct.GetAcctName()).c_str(), 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
                    {
                        for (auto& character : acct.GetChars()) {
                                char label[50];
                                sprintf_s(label, "%s", wstring_to_utf8(character.GetName()).c_str());
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (ImGui::Selectable(label, selected == selectedIndex, ImGuiSelectableFlags_SpanAllColumns)) {
                                    charSelected = charIndex;
                                    acctSelected = acctIndex;
                                    selected = selectedIndex;
                                }
                                ImGui::TableNextColumn();
                                int crealm = character.GetRealm();
                                switch (crealm) {
                                    case 0:
                                        ImGui::Text("Albion");
                                        break;
                                    case 1:
                                        ImGui::Text("Midgard");
                                        break;
                                    case 2:
                                        ImGui::Text("Hibernia");
                                        break;
                                    default:
                                        ImGui::Text("Error");
                                        break;
                                }
                                ImGui::TableNextColumn();
                                auto stmp = magic_enum::enum_cast<ServerName>(character.GetServer());
                                if (stmp.has_value()) {
                                    auto tmpStr = magic_enum::enum_name(stmp.value());
                                    ImGui::Text(static_cast<std::string>(tmpStr).c_str());
                                }
                                selectedIndex++;
                                charIndex++;
                                
                        }
                        ImGui::EndTable();
                        
                    }
                }
                else {
                    ImGui::Separator();
                }
                acctIndex++;
            }
            ImGui::EndChild();
        }

        if (teamWin) {

            ImGui::BeginChild("##teamscreen");
            //First clear the character and account list
            charList.clear();
            acctList.clear();
            acctListIndex.clear();
            teamList.clear();

            //Populate the account list
            FetchAccounts(acctList);
            FetchTeams(teamList);

            //For each account draw a list of columns
            //Count the number of characters in acctList https://stackoverflow.com/questions/22269435/how-to-iterate-through-a-list-of-objects-in-c
            //Keep an index to store for later lookup
            int charCount = 0;
            int acctIndex = 0;
            int charIndex = 0;

            static int charSelected;
            static int acctSelected;

            //Used to store selection of characters
            static std::vector<bool> c_selection;

            //Stores the current teamList index for selection to login
            static int teams_current_idx = 0;

            static char teamname[64] = "";

            //Login button
            static int teamLoginClicked = 0;
            if (ImGui::Button("Login"))
                teamLoginClicked++;
            if (teamLoginClicked & 1)
            {
                Team currTeam = teamList[teams_current_idx];
                for (auto& member : currTeam.GetMembers()) {
                    LaunchDaoc(member.GetAccount().GetChars()[member.GetCharIndex()].GetServer(), 
                        WstringToWchar(member.GetAccount().GetAcctName()),
                        WstringToWchar(member.GetAccount().GetAcctPassword()), 
                        WstringToWchar(member.GetAccount().GetChars()[member.GetCharIndex()].GetName()),
                        member.GetAccount().GetChars()[member.GetCharIndex()].GetRealm());
                    Sleep(1500);
                }

                teamLoginClicked++;
            }
            ImGui::SameLine();
            ImGui::Text("Select Team");
                
            static const char* current_item = NULL;
                
            if (teamList.size() > 0) {
                ImGui::SameLine();

                if (ImGui::BeginCombo("##teamlist", wstring_to_utf8(teamList[teams_current_idx].GetName()).c_str(), ImGuiComboFlags_PopupAlignLeft))
                {
                        
                    for (int n = 0; n < teamList.size(); n++)
                    {
                        const bool is_selected = (teams_current_idx == n);
                        if (ImGui::Selectable(wstring_to_utf8(teamList[n].GetName()).c_str(), is_selected))
                            teams_current_idx = n;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }

            static int createTeamClicked = 0;
            if (ImGui::Button("Save Team"))
                createTeamClicked++;
            if (createTeamClicked & 1)
            {
                Team newTeam = { };
                newTeam.ChangeName(charToWChar(teamname));
                for (int i = 0; i < c_selection.size(); i++) {
                    if (c_selection[i]) {
                        int acctCharIndex = 0;
                        int charCounter = 0;
                        //Get char list of the character
                        for (auto& character : acctList[i].GetChars()) {
                            if (character.GetName() == charList[i].GetName() && character.GetServer() == charList[i].GetServer()) {
                                acctCharIndex = charCounter;
                                break;
                            }
                            charCounter++;
                        }
                        TeamMember tmp = TeamMember{ acctList[i] , acctCharIndex };
                        newTeam.AddMember(tmp);
                    }
                }
                SaveTeam(newTeam);

                /*
                LaunchDaoc(acctList[acctSelected].GetChars()[charIndex].GetServer(), WstringToWchar(acctList[acctSelected].GetAcctName()),
                    WstringToWchar(acctList[acctSelected].GetAcctPassword()), WstringToWchar(acctList[acctSelected].GetChars()[charIndex].GetName()),
                    acctList[acctSelected].GetChars()[charIndex].GetRealm());*/
                createTeamClicked++;
            }

            //Keep track of acctList that matches with the given charList index
            int teamAcctCount = 0;
            int teamCharCount = 0;

            ImGui::SameLine();
            ImGui::InputText("##teamname", teamname, IM_ARRAYSIZE(teamname));
                
            //Maybe we need to loop through and count characters to build the selection[] array
            for (auto& acct : acctList) {
                for (auto& character : acct.GetChars()) {
                    c_selection.push_back(false);
                    acctListIndex.push_back(teamAcctCount);
                    charList.push_back(character);
                    teamCharCount++;
                }
                teamAcctCount++;
            }
            if (ImGui::BeginTable("##teamsCharacters", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {

                for (int n = 0; n < teamCharCount; n++)
                {
                    char buf[32];
                    sprintf_s(buf, "%s", wstring_to_utf8(charList[n].GetName()).c_str());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(buf, c_selection[n], ImGuiSelectableFlags_SpanAllColumns)) {
                        c_selection[n] = !c_selection[n];
                    }
                    ImGui::TableNextColumn();;
                    int crealm = charList[n].GetRealm();
                    switch (crealm) {
                    case 0:
                        ImGui::Text("Albion");
                        break;
                    case 1:
                        ImGui::Text("Midgard");
                        break;
                    case 2:
                        ImGui::Text("Hibernia");
                        break;
                    default:
                        ImGui::Text("Error");
                        break;
                    }
                    ImGui::TableNextColumn();
                    auto stmp = magic_enum::enum_cast<ServerName>(charList[n].GetServer());
                    if (stmp.has_value()) {
                        auto tmpStr = magic_enum::enum_name(stmp.value());
                        ImGui::Text(static_cast<std::string>(tmpStr).c_str());
                    }
                    ImGui::TableNextColumn();
                    ImGui::PushID("##acct" + n);
                    ImGui::Text(wstring_to_utf8(acctList[acctListIndex[n]].GetAcctName()).c_str());
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }


        ImGui::End();
        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = NULL;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != NULL)
    {
        ID3D12InfoQueue* pInfoQueue = NULL;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (g_fenceEvent == NULL)
        return false;

    {
        IDXGIFactory4* dxgiFactory = NULL;
        IDXGISwapChain1* swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, NULL); g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
    if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
    if (g_fence) { g_fence->Release(); g_fence = NULL; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = NULL;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = NULL;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
