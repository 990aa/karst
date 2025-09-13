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
    bool CaptureRegion(RECT rect);
    std::string GetBase64String();
    HBITMAP GetBitmap() const { return m_hBitmap; }

private:
    HBITMAP m_hBitmap;
    int m_width;
    int m_height;

    void Cleanup();
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};
