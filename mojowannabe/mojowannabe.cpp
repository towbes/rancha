// mojowannabe.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "mojowannabe.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//I don't think we actually use this
LRESULT result = 0;
//Global for the ok button so we can tell it was pressed
HWND hWndOkBtn = NULL;
HWND hWndSaveBtn = NULL;
//Global for serverIndex and realmIndex so we can pass it
int serverIndex = 0;
int realmIndex = 0;
//Global for the edit boxes so we can grab values
HWND hWndAcctEdit = NULL;
HWND hWndPassEdit = NULL;
HWND hWndCharEdit = NULL;

//Global for the listview window
HWND hWndListView;

//Globals for pointers to pass to Launch function
wchar_t* acctPtr = NULL;
wchar_t* passPtr = NULL;
wchar_t* charPtr = NULL;

//Global account list
std::vector<Account> acctList;
std::vector<Character> charList{ Character(L"Blank", 1, 1)};


//Globals to save temporary account data
//Account tmpAcct;
//Character tmpChar;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOJOWANNABE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOJOWANNABE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//Callback to clear inside of window https://stackoverflow.com/questions/30786481/winapi-c-how-to-completeley-wipe-clear-everything-in-window
BOOL CALLBACK DestoryChildCallback(
    HWND   hwnd,
    LPARAM lParam
)
{
    if (hwnd != NULL) {
        DestroyWindow(hwnd);
    }

    return TRUE;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOJOWANNABE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOJOWANNABE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   int xpos = 50;            // Horizontal position of the window.
   int ypos = 50;            // Vertical position of the window.
   int nheight = 750;          // Width of the window
   int nwidth = 500;           // Height of the window  
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      xpos, ypos, nheight, nwidth, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   //Draw the login page on initial window
   DrawLogin(hWnd);

   UpdateWindow(hWnd);



   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    
    bool wasHandled = false;
    switch (message)
    {
    case WM_COMMAND:
        {

            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_FILE_LOGINPAGE: 
            {
                DrawLogin(hWnd);
            }
                break;

            case ID_FILE_PROFILES:
            {
                //Delete all the view items first
                ListView_DeleteAllItems(hWndListView);
                //Draw the profiles Window
                DrawProfiles(hWnd);
            }
                break;
            //Parse selection from combo buttons
            case CMB_Server: {
                //If user makes selection from list:
                //Send CB_GETCURSEL message to get the index of seslected item
                //Send CB_GETLBTEXT message to get the item
                // Save the reuslt to serverIndex variable
                serverIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                    (WPARAM)0, (LPARAM)0);
                //wchar_t buffer[3];
                //wsprintfW(buffer, L"%d", serverIndex);
                //MessageBox(hWnd, buffer, TEXT("Item Selected"), MB_OK);
                wasHandled = true;
                result = 0;
            }
                break;
            case CMB_Realm: {
                //Save the index of the realm dropdown combo
                realmIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                    (WPARAM)0, (LPARAM)0);
                //wchar_t newbuffer[3];
                //wsprintfW(newbuffer, L"%d", serverIndex);
                //MessageBox(hWnd, newbuffer, TEXT("Item Selected"), MB_OK);
                wasHandled = true;
                result = 0;
            }
                break;
            //Process a login
            case BTN_LOGIN: {
                //Get the text from account and password fields then pass to launchDAOC along with server value https://stackoverflow.com/questions/57274974/is-it-possible-to-store-text-input-from-a-win32-edit-control-in-a-c-stdstrin
                //Might be a better way to avoid doing this and getting blanks
                acctPtr = GetText(hWndAcctEdit);
                passPtr = GetText(hWndPassEdit);
                charPtr = GetText(hWndCharEdit);
                //If length of acctptr is greater than 0 we're coming from login page
                if (wcslen(acctPtr) > 0) {
                //If the character was empty, send a null charptr and realm index
                    if (wcslen(charPtr) == 0) {
                        LaunchDaoc(serverIndex, acctPtr, passPtr, NULL, NULL);
                    }
                    else {
                        LaunchDaoc(serverIndex, acctPtr, passPtr, charPtr, realmIndex);
                    }
                }
                //Otherwise it's from profile page
                else {
                    // Get the first selected item
                    int iPos = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
                    // iPos is the index of a selected item
                    // do whatever you want with it
                    // get the desired item by its index in the list...
                    // 
                    //Have to initialize LVITEM the same way that it was initialized when set in the itemlist
                    LVITEM item;
                    item.mask = LVIF_TEXT | LVIF_PARAM;
                    item.cchTextMax = 15;
                    item.iItem = iPos;
                    item.iSubItem = 0;
                    ListView_GetItem(hWndListView, &item);
                    //"Catch" the unique ptr from the listitem creation, link in that section to the post explaining this
                    std::unique_ptr<profileIndex> currIndex( reinterpret_cast<profileIndex*>(item.lParam));
                    
                    //If there are characters in this item
                    if (acctList[currIndex->acctIndex].GetChars().size() > 0) {
                        //if there are items in charlist, delete them
                        if (charList.size() > 0) {
                            charList.clear();
                        }

                        //Parse out all the character information then launch daoc
                        //Have to use the _wcsdup + c_str() to convert to wchar_t*
                        charList = acctList[currIndex->acctIndex].GetChars();
                        int sIndex = charList[currIndex->charIndex].GetServer();
                        wchar_t* acctPtr = _wcsdup(acctList[currIndex->acctIndex].GetAcctName().c_str());
                        wchar_t* passPtr = _wcsdup(acctList[currIndex->acctIndex].GetAcctPassword().c_str());
                        wchar_t* charPtr = _wcsdup(charList[currIndex->charIndex].GetName().c_str());
                        int rIndex = charList[currIndex->charIndex].GetRealm();
                        LaunchDaoc(sIndex, acctPtr, passPtr, charPtr, rIndex);
                    }
                    //Just an account. Where can we get server from?
                    else {
                        //hardcoded server to gaheris for now
                        int sIndex = 11;
                        wchar_t* acctPtr = _wcsdup(acctList[currIndex->acctIndex].GetAcctName().c_str());
                        wchar_t* passPtr = _wcsdup(acctList[currIndex->acctIndex].GetAcctPassword().c_str());
                        LaunchDaoc(sIndex, acctPtr, passPtr, NULL, NULL);
                    }

                }
                
                wasHandled = true;
            }
                break;

            case BTN_SAVE: {
                //Get the text from account and password fields then pass to launchDAOC along with server value https://stackoverflow.com/questions/57274974/is-it-possible-to-store-text-input-from-a-win32-edit-control-in-a-c-stdstrin

                acctPtr = GetText(hWndAcctEdit);
                passPtr = GetText(hWndPassEdit);
                charPtr = GetText(hWndCharEdit);

                //Store the account in a temporary object, then call the SaveChar function
                bool test;
                Account tmpAcct = Account(acctPtr, passPtr);
                if (wcslen(charPtr) != 0) {
                    Character tmpChar = Character(charPtr, serverIndex, realmIndex);
                    test = SaveChar(tmpAcct, tmpChar);
                    if (test) {
                        MessageBox(hWnd, TEXT("Character saved"), TEXT("Item Selected"), MB_OK);
                    }
                    else {
                        MessageBox(hWnd, TEXT("Char Write Failed"), TEXT("Item Selected"), MB_OK);
                    }
                }
                else {
                    test = SaveAcct(tmpAcct);
                    if (test) {
                        MessageBox(hWnd, TEXT("Account saved"), TEXT("Item Selected"), MB_OK);
                    }
                    else {
                        MessageBox(hWnd, TEXT("Acct Write Failed"), TEXT("Item Selected"), MB_OK);
                    }
                }


                wasHandled = true;
            }
                break;

            case IDM_ABOUT: {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            }
                break;
            case IDM_EXIT: {
                DestroyWindow(hWnd);
            }
                break;
            default: {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//Helper function to get the text from an edit box, returns a wchar_t* 
wchar_t* GetText(HWND hWnd) {
    std::wstring winText = L"";
    int potential_length = 0;
    int final_length = 0;


    potential_length = GetWindowTextLengthW(hWnd);
    winText.resize(potential_length + 1);

    final_length = GetWindowTextW(hWnd, &winText[0], potential_length + 1);

    winText.resize(final_length);

    return _wcsdup(winText.c_str());
}

//Helper function to save character information to disc.
//If an account file already exists, it will load the acct data, and add the character to that accounts charList
bool SaveChar(Account acct, Character character) {
    acct.AddCharacter(character);
    //check if file exists
    std::fstream acct_file(L"accounts\\" + acct.GetAcctName(), std::ios::in | std::ios::out | std::ios::binary);
    if (acct_file.good()) {
        //File exists
        acct_file.close();

        //Open accounts\<account name>
        std::ifstream tmp_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);
        //Use cereal to read that into a temporary Account object
        cereal::BinaryInputArchive iarchive(tmp_file);
        Account tmpAcct;
        iarchive(tmpAcct);
        //Add the character to the temporary account list
        tmpAcct.AddCharacter(character);
        tmp_file.close();
        //Open a file for writing
        std::ofstream tmp2_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);
        //Use cereal to write the temporary Account to disk
        cereal::BinaryOutputArchive oarchive(tmp2_file);
        oarchive((Account)tmpAcct);
        tmp2_file.close();
        return true;
    }
    else {
        acct_file.close();
        //File doesn't exist so write the account to disk
        std::ofstream tmp_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);
        //create output archive
        cereal::BinaryOutputArchive oarchive(tmp_file);
        oarchive(acct);
        tmp_file.close();
        return true;
    }

    return false;
}


//Helper function to save account data if character wasn't provided
//9/5/2021 still need to implement the loading of only an account file
bool SaveAcct(Account acct) {
    //check if file exists
    std::fstream acct_file(L"accounts\\" + acct.GetAcctName(), std::ios::in | std::ios::out | std::ios::binary);
    if (acct_file.good()) {
        //File exists
        acct_file.close();
        
        std::ifstream tmp_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);
        cereal::BinaryInputArchive iarchive(tmp_file);
        Account tmpAcct;
        iarchive(tmpAcct);
        tmpAcct.SetAcctPassword(acct.GetAcctPassword());
        tmp_file.close();
        std::ofstream tmp2_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);

        cereal::BinaryOutputArchive oarchive(tmp2_file);
        oarchive(tmpAcct);

        tmp2_file.close();
        return true;
    }
    else {
        acct_file.close();
        //File doesn't exist
        std::ofstream tmp_file(L"accounts\\" + acct.GetAcctName(), std::ios::binary);
        //create output archive
        cereal::BinaryOutputArchive oarchive(tmp_file);
        oarchive((Account)acct);
        tmp_file.close();
        return true;
    }

    return false;
}

//Function to Launch DAOC through login.dll
int LaunchDaoc(int server, const wchar_t* acct, const wchar_t* password, const wchar_t* character, int realm) {

    //How do we pass the values to main(int argc, wchar_t** argv)
    //107.21.60.95 10622 23 sx3wally XXXXXXXX

    std::wstring serverLookup = L"";

    //Lookup server value
    switch (server) {
        //ywain1
        case 1:
            serverLookup = L"107.23.173.143 10622 41";
            break;
            //ywain2
        case 2:
            serverLookup = L"107.23.173.143 10622 49";
            break;
            //ywain3
        case 3:
            serverLookup = L"107.23.173.143 10622 50";
            break;
            //ywain4
        case 4:
            serverLookup = L"107.23.173.143 10622 51";
            break;
            //ywain5
        case 5:
            serverLookup = L"107.23.173.143 10622 52";
            break;
            //ywain6
        case 6:
            serverLookup = L"107.23.173.143 10622 53";
            break;
            //ywain7
        case 7:
            serverLookup = L"107.23.173.143 10622 54";
            break;
            //ywain8
        case 8:
            serverLookup = L"107.23.173.143 10622 55";
            break;
            //ywain9
        case 9:
            serverLookup = L"107.23.173.143 10622 56";
            break;
            //ywain10
        case 10:
            serverLookup = L"107.23.173.143 10622 57";
            break;
        //gaheris
        case 11:
            serverLookup = L"107.21.60.95 10622 23";
            break;
    }

    //Build the command line options wstring
    std::wstring options;
    if (character == NULL) {
        options = serverLookup + L" " + acct + L" " + password;
    }
    else {
        options = serverLookup + L" " + acct + L" " + password + L" " + character + L" " + std::to_wstring(realm);
    }
    
    //Convert wstring to wchar_t*
    wchar_t* cmdptr = _wcsdup(options.c_str());

    //C:\\Users\\ritzgames\\source\\repos\\mojowannabe\\Debug\\

    //Open login.dll with the specified options
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo; 
    if (CreateProcess(L"login.dll", cmdptr, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
    {
        //WaitForSingleObject(processInfo.hProcess, INFINITE);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    return 0;
}

//Helper function to load accounts into vector acctList 
void FetchAccounts(std::vector<Account>& acctList) {

    //Current working directory / accounts folder
    std::wstring path = L"accounts\\";

    //Make sure the directory exists, if it doesn't create it
    if (CreateDirectory(path.c_str(), NULL) ||
        ERROR_ALREADY_EXISTS == GetLastError())
    {
        // CopyFile(...)
    }
    else
    {
        MessageBoxW(0, TEXT("Failed to create directory"), TEXT("Error"), MB_OK);
    }

    //Iterate over the files in the directory.  Catch errors
    //Read in the Account objects with cereal
    //Add the account objects to the acctList
    try {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::ifstream tmp_file(entry.path(), std::ios::binary);
        cereal::BinaryInputArchive iarchive(tmp_file);
        Account tmpAcct;
        iarchive(tmpAcct);
        acctList.push_back(tmpAcct);
        tmp_file.close();
    }
    }
    catch (const std::exception& exc)
    {
        size_t size = strlen(exc.what()) + 1;
        wchar_t* etext = new wchar_t[size];

        size_t outSize;
        mbstowcs_s(&outSize, etext, size, exc.what(), size-1);
        // catch anything thrown within try block that derives from std::exception
        MessageBoxW(0, etext, TEXT("Error"), MB_OK);
    }

}

// InitListViewColumns: Adds columns to a list-view control.
// hWndListView:        Handle to the list-view control. 
// Returns TRUE if successful, and FALSE otherwise. 
BOOL InitListViewColumns(HWND hWndListView)
{
    WCHAR szText[256];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol;
    int C_COLUMNS = 3;

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < C_COLUMNS; iCol++)
    {
        lvc.iSubItem = iCol;
        lvc.pszText = szText;
        lvc.cx = 100;               // Width of column in pixels.

        if (iCol < 2)
            lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
            lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Load the names of the column headings from the string resources.
        LoadStringW(HINST_THISCOMPONENT,
            104 + iCol,
            szText,
            sizeof(szText) / sizeof(szText[0]));

        // Insert the columns into the list view.
        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
            return FALSE;
    }

    return TRUE;
}

//From MSDN didn't use this
BOOL InsertListViewItems(HWND hWndListView, LVITEM item)
{
    LVITEM lvI;

    // Initialize LVITEM members that are common to all items.
    lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem = 0;
    lvI.state = 0;

    // Initialize LVITEM members that are different for each item.
    for (int index = 0; index < 1; index++)
    {
        item.iItem = index;
        item.iImage = index;

        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}

//Draws the profiles window
void DrawProfiles(HWND hWnd) {

    //Clear current window contents
    EnumChildWindows(hWnd /* parent hwnd*/, DestoryChildCallback, NULL);
    
    //Clear the acctList vector
    acctList.clear();
    FetchAccounts(acctList);

    //Draw the Profile window
    int xpos = 100;            // Horizontal position of the window.
    int ypos = 75;            // Vertical position of the window.
    int nheight = 20;          // Width of the window
    int nwidth = 70;           // Height of the window  

    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect(hWnd, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    hWndListView = CreateWindowEx(NULL, WC_LISTVIEW, L"LIST",
        WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE,
        0, 0, 
        rcClient.right - rcClient.left - 50,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDM_PROFILE_LIST,
        HINST_THISCOMPONENT,
        NULL);

    //Set position of login button
    xpos = rcClient.right - 50;
    ypos = rcClient.top + 50;
    nwidth = 50;
    nheight = 50;
    // Create the list-view window in report view with label editing enabled.
    hWndOkBtn = CreateWindowEx(0, L"BUTTON", L"Login",
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        xpos, ypos, nwidth, nheight, hWnd, (HMENU)BTN_LOGIN, HINST_THISCOMPONENT,
        NULL);

    //Create the columns
    //Names are specified as string resources
    InitListViewColumns(hWndListView);

    //Add styles to select full rows.  Maybe more stuff we can do here
    //https://cboard.cprogramming.com/windows-programming/90641-add-button-listview.html
    DWORD exStyles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    ListView_SetExtendedListViewStyle(hWndListView, exStyles);

    //Count the number of characters in acctList https://stackoverflow.com/questions/22269435/how-to-iterate-through-a-list-of-objects-in-c
    //Keep an index to store for later lookup
    int charCount = 0;
    int acctIndex = 0;
    int charIndex = 0;
    
    //For each character in each account
    for (auto& acct : acctList) {
        charIndex = 0;
        //if there are characters
        if (acct.GetChars().size() > 0) {
            for (auto& character: acct.GetChars()) {
                //we can assign custom data structure to lParam of LVITEM item https://stackoverflow.com/questions/39481079/c-win32-tracking-listview-items-for-private-chat
                //Set the mask to include LVIF_PARAM https://stackoverflow.com/questions/3473511/how-can-i-attach-userdata-to-each-item-in-a-listview-c-win32
                //use a pointer to a vector of ints for acct index and char index
            
                //old pointer that i think was causing errors in release version
                //profileIndex *tmpIndex = new profileIndex{ acctIndex, charIndex };
                // 
                //Try using smart pointer https://docs.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-160
                std::unique_ptr<profileIndex> tmpIndex(new profileIndex{ acctIndex, charIndex });
                LVITEM item;
                //converting wstring to LPWSTR with _wcsdup
                item.iSubItem = 0;
                item.mask = LVIF_TEXT | LVIF_PARAM;
                item.cchTextMax = 15;
                item.pszText = _wcsdup(acct.GetAcctName().c_str());
                item.iItem = charCount;
                item.lParam = reinterpret_cast<LPARAM>(tmpIndex.get());
                ListView_InsertItem(hWndListView, &item);
                //Need to add if statement to check if we're loading just an account (no character data)
                ListView_SetItemText(hWndListView, charCount, 1, _wcsdup(character.GetName().c_str()));
                //Convert the realm to a string
                switch (character.GetRealm()) {
                case 1:
                    ListView_SetItemText(hWndListView, charCount, 2, (LPWSTR)L"Albion");
                    break;
                case 2:
                    ListView_SetItemText(hWndListView, charCount, 2, (LPWSTR)L"Midgard");
                    break;
                case 3:
                    ListView_SetItemText(hWndListView, charCount, 2, (LPWSTR)L"Hibernia");
                    break;
                }
                charCount += 1;
                charIndex += 1;
                //release the unique ptr
                tmpIndex.release();
            }
        }
        //Only an account
        else {
            std::unique_ptr<profileIndex> tmpIndex(new profileIndex{ acctIndex, 0 });
            LVITEM item;
            //converting wstring to LPWSTR with _wcsdup
            item.iSubItem = 0;
            item.mask = LVIF_TEXT | LVIF_PARAM;
            item.cchTextMax = 15;
            item.pszText = _wcsdup(acct.GetAcctName().c_str());
            item.iItem = charCount;
            item.lParam = reinterpret_cast<LPARAM>(tmpIndex.get());
            ListView_InsertItem(hWndListView, &item);
            charCount += 1;
            tmpIndex.release();
        }
        acctIndex += 1;
    }
    
    //Loop over the number of characters and insert items into the list
    // items (account name) ListView_InsertItem(hWndListView, &item);
    //For subitems Use ListView_SetItemText(hListView, 0, 1, TEXT("12345"));
    //Might need to put a acct index and char index to lookup the account info
    //THen send to LaunchDAOC

}


void DrawLogin(HWND hWnd) {

    //Clear current window contents
    EnumChildWindows(hWnd /* parent hwnd*/, DestoryChildCallback, NULL);

    //Draw the login window
    int xpos = 100;            // Horizontal position of the window.
    int ypos = 75;            // Vertical position of the window.
    int nheight = 20;          // Width of the window
    int nwidth = 70;           // Height of the window     
    HWND hwndParent = hWnd; // Handle to the parent window
    //Account name box

    HWND hWndAcctBox = CreateWindow(TEXT("STATIC"), TEXT("Account:"),
        WS_CHILD | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);

    nwidth = 200;
    xpos = 200;
    hWndAcctEdit = CreateWindow(WC_EDITW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);

    nwidth = 70;
    xpos = 100;
    ypos = ypos + 25;
    HWND hWndPassBox = CreateWindow(TEXT("STATIC"), TEXT("Password:"),
        WS_CHILD | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);

    nwidth = 200;
    xpos = 200;
    hWndPassEdit = CreateWindow(WC_EDITW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);


    nwidth = 70;
    xpos = 100;
    ypos = ypos + 25;
    HWND hWndCharBox = CreateWindow(TEXT("STATIC"), TEXT("Chararcter:"),
        WS_CHILD | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);

    nwidth = 200;
    xpos = 200;
    hWndCharEdit = CreateWindow(WC_EDITW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);

    //Combobox with Realm selection https://docs.microsoft.com/en-us/windows/win32/controls/create-a-simple-combo-box

    xpos = 100;
    ypos = ypos + 25;
    nwidth = 200;
    nheight = 200;
    HWND hWndRealmBox = CreateWindow(WC_COMBOBOX, TEXT(""),
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)CMB_Realm, HINST_THISCOMPONENT,
        NULL);

    //Combobox with server selection https://docs.microsoft.com/en-us/windows/win32/controls/create-a-simple-combo-box

    xpos = 100;
    ypos = ypos + 25;
    nwidth = 200;
    nheight = 250;
    HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)CMB_Server, HINST_THISCOMPONENT,
        NULL);


    ypos = ypos + 25;
    nwidth = 50;
    nheight = 25;
    //Define the button in resource.h then put that definition here https://stackoverflow.com/questions/20640330/winapi-bn-clicked-how-to-identify-which-button-was-clicked
    hWndOkBtn = CreateWindowEx(0, L"BUTTON", L"Login",
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)BTN_LOGIN, HINST_THISCOMPONENT,
        NULL);

    nwidth = 50;
    nheight = 25;
    xpos = xpos + 75;
    //Define the button in resource.h then put that definition here https://stackoverflow.com/questions/20640330/winapi-bn-clicked-how-to-identify-which-button-was-clicked
    hWndSaveBtn = CreateWindowEx(0, L"BUTTON", L"Save",
        WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)BTN_SAVE, HINST_THISCOMPONENT,
        NULL);


    // load the realm combobox with item list.  
    // Send a CB_ADDSTRING message to load each item
    TCHAR Realms[4][10] =
    {
        TEXT(" "), TEXT("Albion"), TEXT("Midgard"), TEXT("Hibernia")
    };

    TCHAR B[16];
    int j = 0;

    memset(&B, 0, sizeof(B));
    for (j = 0; j < 4; j += 1) {
        wcscpy_s(B, sizeof(B) / sizeof(TCHAR), (TCHAR*)Realms[j]);

        //Add the string to the combobox
        SendMessage(hWndRealmBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)B);
    }

    // Send the CB_SETCURSEL message to display an initial item in field
    SendMessage(hWndRealmBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

    // load the server combobox with item list.  
    // Send a CB_ADDSTRING message to load each item
    TCHAR Servers[12][10] =
    {
        TEXT(" "), TEXT("Ywain1"), TEXT("Ywain2"), TEXT("Ywain3"), TEXT("Ywain4"),
        TEXT("Ywain5"), TEXT("Ywain6"), TEXT("Ywain7"), TEXT("Ywain8"),
        TEXT("Ywain9"), TEXT("Ywain10"), TEXT("Gaheris")
    };

    TCHAR A[16];
    int k = 0;

    memset(&A, 0, sizeof(A));
    for (k = 0; k < 12; k += 1) {
        wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Servers[k]);

        //Add the string to the combobox
        SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
    }

    // Send the CB_SETCURSEL message to display an initial item in field
    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}