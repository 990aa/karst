#include <windows.h>

// Global variables
HINSTANCE hInst;
HWND hIconWnd = NULL;
HWND hChatWnd = NULL;
HICON hMainIcon;

// Window class names
const wchar_t ICON_CLASS_NAME[] = L"FloatingIconClass";
const wchar_t CHAT_CLASS_NAME[] = L"ChatWindowClass";

// Forward declarations of functions included in this code module
LRESULT CALLBACK IconWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChatWndProc(HWND, UINT, WPARAM, LPARAM);
void CreateFloatingIcon();
void CreateChatWindow();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    hMainIcon = LoadIcon(NULL, IDI_APPLICATION);

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

    // Create the windows
    CreateFloatingIcon();
    CreateChatWindow();

    ShowWindow(hIconWnd, nCmdShow);
    UpdateWindow(hIconWnd);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void CreateFloatingIcon() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iconX = screenWidth - 48; // 32 icon width + 16 padding
    int iconY = 16;

    hIconWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        ICON_CLASS_NAME,
        L"Floating Icon",
        WS_POPUP,
        iconX, iconY, 32, 32,
        NULL, NULL, hInst, NULL
    );

    SetLayeredWindowAttributes(hIconWnd, 0, 200, LWA_ALPHA);
}

void CreateChatWindow() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int chatX = screenWidth - 416; // 400 window width + 16 padding
    int chatY = 64;

    hChatWnd = CreateWindowExW(
        0,
        CHAT_CLASS_NAME,
        L"Chat Window",
        WS_OVERLAPPEDWINDOW,
        chatX, chatY, 400, 600,
        NULL, NULL, hInst, NULL
    );

    // Create controls
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, 
                  10, 10, 360, 480, hChatWnd, (HMENU)1, hInst, NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 
                  10, 500, 260, 30, hChatWnd, (HMENU)2, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Send", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                  280, 500, 90, 30, hChatWnd, (HMENU)3, hInst, NULL);
}

LRESULT CALLBACK IconWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawIcon(hdc, 0, 0, hMainIcon);
        EndPaint(hWnd, &ps);
    }
    break;
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
    case WM_COMMAND:
        if (LOWORD(wParam) == 3) { // Send button
            // Get text from input and add to message area
            HWND hInput = GetDlgItem(hWnd, 2);
            HWND hMessages = GetDlgItem(hWnd, 1);
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
        }
        break;
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
