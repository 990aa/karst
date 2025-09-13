#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <gdiplus.h>

class CScreenCapture {
public:
    CScreenCapture();
    ~CScreenCapture();

    bool CaptureFullScreen();
    std::string GetBase64String();

private:
    HBITMAP m_hBitmap;

    void Cleanup();
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};