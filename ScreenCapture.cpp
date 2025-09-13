#include "ScreenCapture.h"
#include <windows.h>
#include <wincrypt.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "crypt32.lib")

CScreenCapture::CScreenCapture() : m_hBitmap(NULL) {}

CScreenCapture::~CScreenCapture() {
    Cleanup();
}

void CScreenCapture::Cleanup() {
    if (m_hBitmap) {
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;
    }
}

bool CScreenCapture::CaptureFullScreen() {
    Cleanup();

    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    m_hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    SelectObject(hMemoryDC, m_hBitmap);

    BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, screenX, screenY, SRCCOPY);

    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return m_hBitmap != NULL;
}

std::string CScreenCapture::GetBase64String() {
    if (!m_hBitmap) return "";

    Gdiplus::Bitmap bitmap(m_hBitmap, NULL);
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) < 0) return "";

    IStream* pStream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &pStream);

    bitmap.Save(pStream, &pngClsid, NULL);

    HGLOBAL hGlobal = NULL;
    GetHGlobalFromStream(pStream, &hGlobal);

    LPVOID pData = GlobalLock(hGlobal);
    SIZE_T size = GlobalSize(hGlobal);

    std::string base64String;
    DWORD strSize = 0;
    CryptBinaryToStringA((BYTE*)pData, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &strSize);
    base64String.resize(strSize);
    CryptBinaryToStringA((BYTE*)pData, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64String[0], &strSize);

    GlobalUnlock(hGlobal);
    pStream->Release();

    return base64String;
}

int CScreenCapture::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}
