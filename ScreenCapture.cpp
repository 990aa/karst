#include "ScreenCapture.h"
#include <wincrypt.h>
#include <objidl.h>
#include <vector>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "crypt32.lib")

CScreenCapture::CScreenCapture() : m_hBitmap(NULL), m_width(0), m_height(0) {}

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
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    RECT screenRect = {screenX, screenY, screenX + screenWidth, screenY + screenHeight};
    return CaptureRegion(screenRect);
}

bool CScreenCapture::CaptureRegion(RECT rect) {
    Cleanup();

    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) return false;

    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    m_hBitmap = CreateCompatibleBitmap(hScreenDC, m_width, m_height);
    if (!m_hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, m_hBitmap);

    if (!BitBlt(hMemoryDC, 0, 0, m_width, m_height, hScreenDC, rect.left, rect.top, CAPTUREBLT | SRCCOPY)) {
        Cleanup();
    }

    SelectObject(hMemoryDC, hOldBitmap);
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
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) != S_OK) return "";

    if (bitmap.Save(pStream, &pngClsid, NULL) != Gdiplus::Ok) {
        pStream->Release();
        return "";
    }

    HGLOBAL hGlobal = NULL;
    if (GetHGlobalFromStream(pStream, &hGlobal) != S_OK) {
        pStream->Release();
        return "";
    }

    LPVOID pData = GlobalLock(hGlobal);
    if (!pData) {
        pStream->Release();
        return "";
    }

    SIZE_T size = GlobalSize(hGlobal);
    std::vector<byte> buffer(size);
    memcpy(buffer.data(), pData, size);

    GlobalUnlock(hGlobal);
    pStream->Release();

    DWORD strSize = 0;
    CryptBinaryToStringA(buffer.data(), buffer.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &strSize);
    if (strSize == 0) return "";

    std::string base64String;
    base64String.resize(strSize);
    CryptBinaryToStringA(buffer.data(), buffer.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64String[0], &strSize);
    
    // Resize to actual size, as CryptBinaryToStringA includes null terminator in size
    base64String.resize(strSize - 1);

    return base64String;
}

int CScreenCapture::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;

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
