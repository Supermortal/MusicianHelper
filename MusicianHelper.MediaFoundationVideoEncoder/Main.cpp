#include <iostream>
#include "VideoEncoder.h"

int main(int argc, char* argv[]) {
    system("pause");

    HRESULT hr = S_OK;

    VideoSettings vs;
    vs.videoFps = 60;
    vs.videoBitRate = 800000;
    vs.videoFps = 30;
    vs.videoEncodingFormat = MFVideoFormat_WMV3;

    //VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", L"output.wmv", vs);
    VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp", L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav", L"output.wmv", vs);

    ve->Encode();

    return SUCCEEDED(hr) ? 0 : 1;
}
