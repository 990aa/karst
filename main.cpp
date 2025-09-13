#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include "ScreenCapture.h"

// Global variables
HINSTANCE hInst;
HWND hIconWnd = NULL;
HWND hChatWnd = NULL;
HICON hMainIcon;
ULONG_PTR gdiplusToken;

std::vector<HBITMAP> g_thumbnails;
std::vector<std::string> g_base64Images;

// Window class names
const wchar_t ICON_CLASS_NAME[] = L"FloatingIconClass";
const wchar_t CHAT_CLASS_NAME[] = L"ChatWindowClass";

// Control IDs
#define IDC_MESSAGE_AREA 1
#define IDC_INPUT_FIELD 2
#define IDC_SEND_BUTTON 3
#define IDC_CAPTURE_BUTTON 4
#define IDC_THUMBNAIL_LIST 5

// Forward declarations
LRESULT CALLBACK IconWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChatWndProc(HWND, UINT, WPARAM, LPARAM);
void CreateFloatingIcon();
void CreateChatWindow();
void AddThumbnail(HBITMAP hBitmap);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    hMainIcon = LoadIcon(NULL, IDI_APPLICATION);

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Register window classes
    WNDCLASSEXW wcexIcon = {0};
    wcexIcon.cbSize = sizeof(WNDCLASSEXW);
    wcexIcon.style = CS_HREDRAW | CS_VREDRAW;
    wcexIcon.lpfnWndProc = IconWndProc;
    wcexIcon.hInstance = hInstance;
    wcexIcon.hIcon = hMainIcon;
    wcexIcon.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcexIcon.lpszClassName = ICON_CLASS_NAME;
    RegisterClassExW(&wcexIcon);

    WNDCLASSEXW wcexChat = {0};
    wcexChat.cbSize = sizeof(WNDCLASSEXW);
    wcexChat.style = CS_HREDRAW | CS_VREDRAW;
    wcexChat.lpfnWndProc = ChatWndProc;
    wcexChat.hInstance = hInstance;
    wcexChat.hIcon = hMainIcon;
    wcexChat.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcexChat.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcexChat.lpszClassName = CHAT_CLASS_NAME;
    RegisterClassExW(&wcexChat);

    CreateFloatingIcon();
    CreateChatWindow();

    ShowWindow(hIconWnd, nCmdShow);
    UpdateWindow(hIconWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Shutdown GDI+
    Gdiplus::GdiplusShutdown(gdiplusToken);
    
    // Cleanup thumbnails
    for (HBITMAP hBmp : g_thumbnails) {
        DeleteObject(hBmp);
    }

    return (int)msg.wParam;
}

void CreateFloatingIcon() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iconX = screenWidth - 48;
    int iconY = 16;

    hIconWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        ICON_CLASS_NAME, L"Floating Icon", WS_POPUP,
        iconX, iconY, 32, 32, NULL, NULL, hInst, NULL);

    SetLayeredWindowAttributes(hIconWnd, 0, 200, LWA_ALPHA);
}

void CreateChatWindow() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int chatX = screenWidth - 416;
    int chatY = 64;

    hChatWnd = CreateWindowExW(
        0, CHAT_CLASS_NAME, L"Chat Window", WS_OVERLAPPEDWINDOW,
        chatX, chatY, 400, 600, NULL, NULL, hInst, NULL);

    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 
                  10, 10, 360, 400, hChatWnd, (HMENU)IDC_MESSAGE_AREA, hInst, NULL);
    CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_HASSTRINGS,
                  10, 420, 360, 60, hChatWnd, (HMENU)IDC_THUMBNAIL_LIST, hInst, NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 
                  10, 500, 220, 30, hChatWnd, (HMENU)IDC_INPUT_FIELD, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Capture", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                  240, 500, 60, 30, hChatWnd, (HMENU)IDC_CAPTURE_BUTTON, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Send", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                  310, 500, 60, 30, hChatWnd, (HMENU)IDC_SEND_BUTTON, hInst, NULL);
}

LRESULT CALLBACK IconWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawIcon(hdc, 0, 0, hMainIcon);
        EndPaint(hWnd, &ps);
    } break;
    case WM_LBUTTONDOWN:
        ShowWindow(hChatWnd, SW_SHOW);
        SetForegroundWindow(hChatWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ChatWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_SEND_BUTTON: {
            HWND hInput = GetDlgItem(hWnd, IDC_INPUT_FIELD);
            HWND hMessages = GetDlgItem(hWnd, IDC_MESSAGE_AREA);
            int len = GetWindowTextLength(hInput) + 1;
            if (len > 1 || !g_base64Images.empty()) {
                // Append text
                if (len > 1) {
                    wchar_t* text = new wchar_t[len];
                    GetWindowTextW(hInput, text, len);
                    int msgLen = GetWindowTextLength(hMessages);
                    SendMessageW(hMessages, EM_SETSEL, (WPARAM)msgLen, (LPARAM)msgLen);
                    if(msgLen > 0) SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
                    SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)text);
                    SetWindowTextW(hInput, L"");
                    delete[] text;
                }
                // Append image placeholders
                for (size_t i = 0; i < g_base64Images.size(); ++i) {
                    int msgLen = GetWindowTextLength(hMessages);
                    SendMessageW(hMessages, EM_SETSEL, (WPARAM)msgLen, (LPARAM)msgLen);
                    if(msgLen > 0) SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
                    std::wstring placeholder = L"[Image " + std::to_wstring(i + 1) + L" attached]";
                    SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)placeholder.c_str());
                }

                // Clear thumbnails and data
                SendDlgItemMessage(hWnd, IDC_THUMBNAIL_LIST, LB_RESETCONTENT, 0, 0);
                for (HBITMAP hBmp : g_thumbnails) DeleteObject(hBmp);
                g_thumbnails.clear();
                g_base64Images.clear();
            }
        } break;
        case IDC_CAPTURE_BUTTON: {
            ShowWindow(hChatWnd, SW_HIDE);
            Sleep(300); // Give time for window to hide

            CScreenCapture capture;
            if (capture.CaptureFullScreen()) {
                g_base64Images.push_back(capture.GetBase64String());
                
                // For simplicity, we'll just add a string to the listbox
                std::wstring thumbText = L"Image " + std::to_wstring(g_base64Images.size());
                SendDlgItemMessage(hWnd, IDC_THUMBNAIL_LIST, LB_ADDSTRING, 0, (LPARAM)thumbText.c_str());
            } else {
                MessageBox(hWnd, L"Screen capture failed.", L"Error", MB_OK | MB_ICONERROR);
            }

            ShowWindow(hChatWnd, SW_SHOW);
            SetForegroundWindow(hChatWnd);
        } break;
        }
    } break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_MINIMIZE) {
            ShowWindow(hWnd, SW_HIDE);
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
