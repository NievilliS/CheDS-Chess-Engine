/****************************************************
 *  CHESS ENGINE by David Schoosleitner             *
 * Date: 15.08.2022                                 *
 * File: board.h                                    *
 ****************************************************
 * Responsibilities:                                *
 *  - win32 visual interface main                   *
 *                                                  *
 * Requires:                                        *
 *  - ...                                           *
 ****************************************************/

/***** PRODUCTON MODE REQUIRED *****/
//#ifdef PRODUCTION_MODE

/***** INCLUDES *****/
#include <windows.h>
#include "winresources.h"

/***** VARIABLE DECLARATIONS AND DEFINITIONS *****/
LPCSTR class_name = "ChedsWindow1_0_1Class";

/***** WINDOW PROCESS *****/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        case WM_COMMAND:
            switch(LOBYTE(wParam)) {
                case IDRC_NEW:
                    MessageBox(NULL, "New Button pressed!", "Menubutton",
                        MB_ICONHAND | MB_OK);
                break;
                case IDRC_OPEN:
                    MessageBox(NULL, "Open Button pressed!", "Menubutton",
                        MB_ICONHAND | MB_OK);
                break;
                case IDRC_SAVE:
                    MessageBox(NULL, "Save Button pressed!", "Menubutton",
                        MB_ICONHAND | MB_OK);
                break;
                case IDRC_SAVEAS:
                    MessageBox(NULL, "Save As Button pressed!", "Menubutton",
                        MB_ICONHAND | MB_OK);
                break;
                case IDRC_EXIT:
                    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;
            }
        break;
        case WM_PAINT:
        {

        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/***** WINDOWS MAIN *****/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = (HICON) LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 32, 32, 0);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = class_name;
    wc.hIconSm       = (HICON) LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, 0);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        class_name,
        "CHEDS 1.0.1 INTERFACE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
