#include "rancha.h"

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
    //realm Cmd expects: 1= alb, 2=mid, 3=hib
    //incoming realm is is 0/1/2
    realm++;

    //How do we pass the values to main(int argc, wchar_t** argv)
    //107.21.60.95 10622 23 sx3wally XXXXXXXX

    std::wstring serverLookup = L"";

    //Lookup server value
    switch (server) {
        //ywain1
    case 0:
        serverLookup = L"107.23.173.143 10622 41";
        break;
        //ywain2
    case 1:
        serverLookup = L"107.23.173.143 10622 49";
        break;
        //ywain3
    case 2:
        serverLookup = L"107.23.173.143 10622 50";
        break;
        //ywain4
    case 3:
        serverLookup = L"107.23.173.143 10622 51";
        break;
        //ywain5
    case 4:
        serverLookup = L"107.23.173.143 10622 52";
        break;
        //ywain6
    case 5:
        serverLookup = L"107.23.173.143 10622 53";
        break;
        //ywain7
    case 6:
        serverLookup = L"107.23.173.143 10622 54";
        break;
        //ywain8
    case 7:
        serverLookup = L"107.23.173.143 10622 55";
        break;
        //ywain9
    case 8:
        serverLookup = L"107.23.173.143 10622 56";
        break;
        //ywain10
    case 9:
        serverLookup = L"107.23.173.143 10622 57";
        break;
        //gaheris
    case 10:
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
//Pass the vector by reference so that it gets updated
void FetchAccounts(std::vector<Account>& acctList) {

    //First clear the account list
    acctList.clear();

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
        mbstowcs_s(&outSize, etext, size, exc.what(), size - 1);
        // catch anything thrown within try block that derives from std::exception
        MessageBoxW(0, etext, TEXT("Error"), MB_OK);
    }

}

//String helper function
//https://stackoverflow.com/questions/3074776/how-to-convert-char-array-to-wchar-t-array
const wchar_t* charToWChar(const char* text)
{
    std::string tmpStr = text;
    std::wstring wText = std::wstring(tmpStr.begin(), tmpStr.end());
    wchar_t* cmdptr = _wcsdup(wText.c_str());
    return cmdptr;
}

//String helper function
//https://stackoverflow.com/questions/52739477/how-to-return-copy-values-of-an-unique-ptrunsigned-char
//https://stackoverflow.com/questions/21467014/memory-leak-when-using-smart-pointers
const char* WstringToChar(std::wstring text)
{
    const wchar_t* inputW = text.c_str();
    size_t charsConverted = 0;
    wcstombs_s(&charsConverted, nullptr, 0, inputW, text.length());
    auto outputString = std::unique_ptr<char[]>(new char[charsConverted]);

    wcstombs_s(&charsConverted, outputString.get(), charsConverted, inputW, text.length());
    return outputString.get();
}

wchar_t* WstringToWchar(std::wstring text) {
   return _wcsdup(text.c_str());
}