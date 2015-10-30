//http://www.codeproject.com/Tips/140642/Converting-between-different-image-formats-using-G
//http://www.codeproject.com/Tips/139345/Embracing-IStream-as-just-a-stream-of-bytes.aspx
//http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array

#include <iostream>
#include <stdlib.h>
#include <istream>
#include <Windows.h>
#include <gdiplus.h>
#include <fstream>

#pragma comment(lib, "gdiplus")

ULONG_PTR m_gdiplusToken;

Gdiplus::Status imageToImage(
    LPCWSTR fileIn, LPCWSTR fileOut, BSTR wszOutputMimeType)
{
    namespace G = Gdiplus;
    G::Status status = G::Ok;
    G::Image imageSrc(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.jpg");
    status = imageSrc.GetLastStatus();
    if (G::Ok != status) {
        return status;
    }
    UINT numEncoders = 0;
    UINT sizeEncodersInBytes = 0;
    status = G::GetImageEncodersSize(&numEncoders, &sizeEncodersInBytes);
    if (status != G::Ok) {
        return status;
    }
    G::ImageCodecInfo *pImageCodecInfo =
        (G::ImageCodecInfo *) malloc(sizeEncodersInBytes);
    status = G::GetImageEncoders(
        numEncoders, sizeEncodersInBytes, pImageCodecInfo);
    if (status != G::Ok) {
        return status;
    }
    CLSID clsidOut;
    status = G::UnknownImageFormat;
    for (UINT j = 0; j < numEncoders; j++) {
        if (0 == wcscmp(pImageCodecInfo[j].MimeType, wszOutputMimeType)) {
            clsidOut = pImageCodecInfo[j].Clsid;
            status = G::Ok;
            break;
        }
    }
    free(pImageCodecInfo);
    if (status != G::Ok) {
        return status;
    }
    return imageSrc.Save(L"test.bmp", &clsidOut);
}

LPCWSTR convertCharArrToLPCWSTR(char* charArr) {
    size_t origsize = strlen(charArr) + 1;
    const size_t newsize = 200;
    size_t convertedChars = 0;
    wchar_t *wcstring = new wchar_t[newsize];
    mbstowcs_s(&convertedChars, wcstring, newsize, charArr, _TRUNCATE);

    return (LPCWSTR)wcstring;
}

int main(int argc, char* argv[]) {
    //arguments
    //C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\paper-stained-3-texture.jpg test_encode.bmp
    //C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.jpg test_encode.bmp

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    /*const char *inCs = argv[1];
    const char *outCs = argv[2];*/

    /*const size_t newsize = 200;

    size_t origsize = strlen(inCs) + 1;    
    size_t convertedChars = 0;
    wchar_t *inFilePath = new wchar_t[newsize];
    mbstowcs_s(&convertedChars, inFilePath, newsize, inCs, _TRUNCATE);

    origsize = strlen(outCs) + 1;
    convertedChars = 0;
    wchar_t *outFilePath = new wchar_t[newsize];
    mbstowcs_s(&convertedChars, outFilePath, newsize, outCs, _TRUNCATE);
*/

    LPCWSTR inFilePath = convertCharArrToLPCWSTR(argv[1]);
    LPCWSTR outFilePath = convertCharArrToLPCWSTR(argv[2]);

    Gdiplus::Status s = imageToImage((LPCWSTR)inFilePath, (LPCWSTR)outFilePath, L"image/bmp");
    if (Gdiplus::Ok == s) {
        std::cout << "Conversion successful" << std::endl;
    }
    else {
        std::cout << "Conversion failed" << std::endl;
    }

    system("pause");

    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


