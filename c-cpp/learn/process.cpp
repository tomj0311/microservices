#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

int main( int, char *[] )
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "target.exe") == 0)
            {  
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                // Do stuff..

                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);

    return 0;
}