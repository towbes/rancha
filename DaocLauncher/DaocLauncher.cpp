// DaocLauncher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "DaocLauncher.h"

int main()
{
    LPWSTR* szArglist;
    int nArgs;
    //Parse the command line from unicode https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (NULL == szArglist)
    {
        return 0;
    }
    //Not enough arguments supplied
    if (nArgs < 5) {
        MessageBox(NULL, L"Not enough args", L"Error!", MB_ICONERROR | MB_OK);
        return 0;
    }

    std::wstring options = L"game.dll ";
    for (int i = 0; i < nArgs; i++) {
        //First check that the argument isn't *.exe
        size_t len = wcslen(&options[i]);
        if (len >= 4 && !wcscmp(&options[i] + len - 4, L".exe")) {
            /* the extension is .exe do nothing*/
        }
        else {
            //Add the argument to cmd options
            options += szArglist[i];
            //Don't add a space to last argument
            if (i < (nArgs - 1)) {
                options += L" ";
            }
        }

    }
    wchar_t* cmdptr = _wcsdup(options.c_str());

    //Change the directory to the DAOC directory
    SetCurrentDirectory(L"C:\\Program Files (x86)\\Electronic Arts\\Dark Age of Camelot");

    //old hardcoded command this is what should be passed to the command line for launching game.dll (yes game.dll gets written twice)
    //WCHAR lpCommandLine[] = L"game.dll 107.21.60.95 10622 23 sx3wally XXXXXXXX";


    
    //start the game.dll process
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;

    //Call game.dll with the proper command line args
    if (CreateProcess(L"C:\\Program Files (x86)\\Electronic Arts\\Dark Age of Camelot\\game.dll", cmdptr, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
    {
        //Commented out, don't wait for the process
        //WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }

    Sleep(1000);

    //Find and kill mutexmutex
    DWORD pid = FindGameDll();

    //Free the cmd ptr
    free(cmdptr);
    LocalFree(szArglist);
    return 0;
}


DWORD FindGameDll() {
    //From here: https://scorpiosoftware.net/2020/03/15/how-can-i-close-a-handle-in-another-process/
    //Get a snapshot of current processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    // skip the idle process
    ::Process32First(hSnapshot, &pe);

    //Look for game.dll and try to close mutex
    //If we don't find a mutex in game.dll to close, go to the next game.dll process
    //Closing the mutex does not seem to need admin access
    DWORD pid = 0;
    while (::Process32Next(hSnapshot, &pe)) {
        if (::_wcsicmp(pe.szExeFile, L"game.dll") == 0) {
            // found it!
            pid = pe.th32ProcessID;

            //Get the process handle
            HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_DUP_HANDLE,
                FALSE, pid);
            if (!hProcess) {
                MessageBox(NULL, L"Failed to OpenProcess", L"Error!", MB_ICONERROR | MB_OK);
                ::GetLastError();
                return 1;
            }

            //Enumerate handles in the process
            ULONG size = 1 << 10;
            std::unique_ptr<BYTE[]> buffer;
            for (;;) {
                buffer = std::make_unique<BYTE[]>(size);
                auto status = ::NtQueryInformationProcess(hProcess, ProcessHandleInformation,
                    buffer.get(), size, &size);
                if (NT_SUCCESS(status))
                    break;
                if (status == STATUS_INFO_LENGTH_MISMATCH) {
                    size += 1 << 10;
                    continue;
                }
                MessageBox(NULL, L"Error getting handles", L"Error!", MB_ICONERROR | MB_OK);
                return 1;
            }

            //Build our targetName
            WCHAR targetName[256];
            DWORD sessionId;
            ::ProcessIdToSessionId(pid, &sessionId);
            ::swprintf_s(targetName,
                L"\\BaseNamedObjects\\DAoCi1",
                sessionId);
            auto len = ::wcslen(targetName);

            //Build our targetName
            WCHAR targetName2[256];
            DWORD sessionId2;
            ::ProcessIdToSessionId(pid, &sessionId2);
            ::swprintf_s(targetName2,
                L"\\BaseNamedObjects\\DAoCi2",
                sessionId2);
            auto len2 = ::wcslen(targetName2);

            //Loop for all handles, duplicate the handle to our own process
            auto hInfo = reinterpret_cast<PROCESS_HANDLE_SNAPSHOT_INFORMATION*>(buffer.get());
            for (ULONG i = 0; i < hInfo->NumberOfHandles; i++) {
                HANDLE h = hInfo->Handles[i].HandleValue;
                HANDLE hTarget;
                if (!::DuplicateHandle(hProcess, h, ::GetCurrentProcess(), &hTarget,
                    0, FALSE, DUPLICATE_SAME_ACCESS))
                    continue;   // move to next handle
                //Get the name of the handle
                BYTE nameBuffer[1 << 10];
                auto status = ::NtQueryObject(hTarget, ObjectNameInformation,
                    nameBuffer, sizeof(nameBuffer), nullptr);
                auto name = reinterpret_cast<UNICODE_STRING*>(nameBuffer);
                if ((name->Buffer &&
                    ::_wcsnicmp(name->Buffer, targetName, len) == 0) || (name->Buffer &&
                        ::_wcsnicmp(name->Buffer, targetName2, len2) == 0)) {
                    // found it!
                    ::DuplicateHandle(hProcess, h, ::GetCurrentProcess(), &hTarget,
                        0, FALSE, DUPLICATE_CLOSE_SOURCE);
                    ::CloseHandle(hTarget);
                    break;
                }
                if (!NT_SUCCESS(status))
                    continue;
            }
        }
    }
    ::CloseHandle(hSnapshot);
    return pid;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
