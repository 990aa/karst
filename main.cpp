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

std::vector<std::string> g_base64Images;

// Window class names
const wchar_t ICON_CLASS_NAME[] = L"FloatingIconClass";
const wchar_t CHAT_CLASS_NAME[] = L"ChatWindowClass";

// Control IDs
#define IDC_MESSAGE_AREA 1
#define IDC_INPUT_FIELD 2
#define IDC_SEND_BUTTON 3
#define IDC_CAPTURE_BUTTON 4

// Forward declarations
LRESULT CALLBACK IconWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChatWndProc(HWND, UINT, WPARAM, LPARAM);
void CreateFloatingIcon();
void CreateChatWindow();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    hMainIcon = LoadIcon(NULL, IDI_APPLICATION);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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

    Gdiplus::GdiplusShutdown(gdiplusToken);
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
                  10, 10, 360, 480, hChatWnd, (HMENU)IDC_MESSAGE_AREA, hInst, NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 
                  10, 500, 170, 30, hChatWnd, (HMENU)IDC_INPUT_FIELD, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Capture", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                  190, 500, 80, 30, hChatWnd, (HMENU)IDC_CAPTURE_BUTTON, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Send", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                  280, 500, 90, 30, hChatWnd, (HMENU)IDC_SEND_BUTTON, hInst, NULL);
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
        if (wmId == IDC_SEND_BUTTON) {
            HWND hInput = GetDlgItem(hWnd, IDC_INPUT_FIELD);
            HWND hMessages = GetDlgItem(hWnd, IDC_MESSAGE_AREA);
            int len = GetWindowTextLength(hInput) + 1;
            if (len > 1) {
                wchar_t* text = new wchar_t[len];
                GetWindowTextW(hInput, text, len);
                int msgLen = GetWindowTextLength(hMessages);
                SendMessageW(hMessages, EM_SETSEL, (WPARAM)msgLen, (LPARAM)msgLen);
                SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
                SendMessageW(hMessages, EM_REPLACESEL, 0, (LPARAM)text);
                SetWindowTextW(hInput, L"");
                delete[] text;
            }
        } else if (wmId == IDC_CAPTURE_BUTTON) {
            ShowWindow(hChatWnd, SW_HIDE);
            Sleep(300);

            CScreenCapture capture;
            if (capture.CaptureFullScreen()) {
                g_base64Images.push_back(capture.GetBase64String());
                MessageBoxW(hWnd, L"Screen capture successful!", L"Success", MB_OK);
            } else {
                MessageBoxW(hWnd, L"Screen capture failed.", L"Error", MB_OK | MB_ICONERROR);
            }

            ShowWindow(hChatWnd, SW_SHOW);
            SetForegroundWindow(hChatWnd);
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
