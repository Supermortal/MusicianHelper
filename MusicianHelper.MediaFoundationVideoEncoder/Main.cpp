#include <iostream>
#include "VideoEncoder.h"
#include <stdlib.h>

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
	//C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\sorry_dave.wav output.wmv
    //C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav output.wmv

	HRESULT hr = S_OK;

	LPCWSTR *argStrs = new LPCWSTR[argc - 1];
	int index = 0;
	for (int i = 1; i < argc; i++) {
		char * cs = argv[i];
        argStrs[index] = convertCharArrToLPCWSTR(cs);
		index++;
	}

	VideoSettings vs;
	vs.videoBitRate = 800000;
	vs.videoFps = 30;
	vs.videoEncodingFormat = MFVideoFormat_WMV3;

	VideoEncoder *ve = new VideoEncoder(argStrs[0], argStrs[1], argStrs[2], vs);
	delete argStrs;

	time_t t = time(0);   // get time now
	struct tm *now = new tm();
	localtime_s(now, &t);
	/*std::cout << "Start time: " << (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday
		<< " "
		<< now->tm_hour
		<< " "
		<< now->tm_min
		<< std::endl;*/
	ve->Encode();
	t = time(0);   // get time now
	now = new tm();
	localtime_s(now, &t);
	/*std::cout << "End time: " << (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday
		<< " "
		<< now->tm_hour
		<< " "
		<< now->tm_min
		<< std::endl;*/

	/*system("pause");*/
	return SUCCEEDED(hr) ? 0 : 1;
}
