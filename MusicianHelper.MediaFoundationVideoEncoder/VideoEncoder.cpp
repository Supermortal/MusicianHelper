#define _AFXDLL

#include <afx.h>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <atlstr.h>
#include <stack>
#include <iostream>
#include <math.h>
#include <iomanip>

#define WINVER _WIN32_WINNT_WIN7

#include <stdio.h>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Mf")

#include "VideoEncoder.h"

template <int a, int b, int c, int d>
struct FOURCC
{
	static const unsigned int value = (((((d << 8) | c) << 8) | b) << 8) | a;
};

unsigned int id(FOURCC<'a', 'b', 'c', 'd'>::value);

VideoEncoder::VideoEncoder(LPCWSTR imageFilePath, LPCWSTR audioFilePath, LPCWSTR videoOutputPath, VideoSettings vs)
{
	mImageFilePath = imageFilePath;
	mVideoOutputPath = videoOutputPath;
	mAudioFilePath = audioFilePath;
	SetVideoSettings(vs);
}

VideoEncoder::~VideoEncoder()
{

}

bool VideoEncoder::ConvertToDFB(HBITMAP& hBitmap) {
	bool bConverted = false;
	BITMAP stBitmap;
	if (GetObject(hBitmap, sizeof(stBitmap), &stBitmap) && stBitmap.bmBits)
	{
		// that is a DIB. Now we attempt to create
		// a DFB with the same sizes, and with the pixel
		// format of the display (to omit conversions
		// every time we draw it).
		HDC hScreen = GetDC(NULL);
		if (hScreen)
		{
			HBITMAP hDfb =
				CreateCompatibleBitmap(hScreen,
				stBitmap.bmWidth, stBitmap.bmHeight);
			if (hDfb)
			{
				// now let's ensure what we've created is a DIB.
				if (GetObject(hDfb, sizeof(stBitmap),
					&stBitmap) && !stBitmap.bmBits)
				{
					// ok, we're lucky. Now we have
					// to transfer the image to the DFB.
					HDC hMemSrc = CreateCompatibleDC(NULL);
					if (hMemSrc)
					{
						HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
						if (hOldSrc)
						{
							HDC hMemDst = CreateCompatibleDC(NULL);
							if (hMemDst)
							{
								HGDIOBJ hOldDst = SelectObject(hMemDst, hDfb);
								if (hOldDst)
								{
									// transfer the image using BitBlt
									// function. It will probably end in the
									// call to driver's DrvCopyBits function.
									if (BitBlt(hMemDst, 0, 0,
										stBitmap.bmWidth, stBitmap.bmHeight,
										hMemSrc, 0, 0, SRCCOPY))
										bConverted = true; // success

									VERIFY(SelectObject(hMemDst, hOldDst));
								}
								VERIFY(DeleteDC(hMemDst));
							}
							VERIFY(SelectObject(hMemSrc, hOldSrc));
						}
						VERIFY(DeleteDC(hMemSrc));
					}
				}

				if (bConverted)
				{
					VERIFY(DeleteObject(hBitmap)); // it's no longer needed
					hBitmap = hDfb;
				}
				else
					VERIFY(DeleteObject(hDfb));
			}
			ReleaseDC(NULL, hScreen);
		}
	}
	return bConverted;
}

bool VideoEncoder::ConvertToDIB(HBITMAP& hBitmap) {
	bool bConverted = false;
	BITMAP stBitmap;
	if (GetObject(hBitmap, sizeof(stBitmap),
		&stBitmap) && !stBitmap.bmBits)
	{
		// that is a DFB. Now we attempt to create
		// a DIB with the same sizes and pixel format.
		HDC hScreen = GetDC(NULL);
		if (hScreen)
		{
			union {
				BITMAPINFO stBitmapInfo;
				BYTE pReserveSpace[sizeof(BITMAPINFO)
					+ 0xFF * sizeof(RGBQUAD)];
			};
			ZeroMemory(pReserveSpace, sizeof(pReserveSpace));
			stBitmapInfo.bmiHeader.biSize = sizeof(stBitmapInfo.bmiHeader);
			stBitmapInfo.bmiHeader.biWidth = stBitmap.bmWidth;
			stBitmapInfo.bmiHeader.biHeight = -stBitmap.bmHeight;
			stBitmapInfo.bmiHeader.biPlanes = 1;
			stBitmapInfo.bmiHeader.biBitCount = stBitmap.bmBitsPixel;
			stBitmapInfo.bmiHeader.biCompression = BI_RGB;

			if (stBitmap.bmBitsPixel <= 8)
			{
				stBitmapInfo.bmiHeader.biClrUsed =
					1 << stBitmap.bmBitsPixel;
				// This image is paletted-managed.
				// Hence we have to synthesize its palette.
			}
			stBitmapInfo.bmiHeader.biClrImportant =
				stBitmapInfo.bmiHeader.biClrUsed;

			PVOID pBits;
			HBITMAP hDib = CreateDIBSection(hScreen,
				&stBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0);

			if (hDib)
			{
				// ok, we're lucky. Now we have
				// to transfer the image to the DFB.
				HDC hMemSrc = CreateCompatibleDC(NULL);
				if (hMemSrc)
				{
					HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
					if (hOldSrc)
					{
						HDC hMemDst = CreateCompatibleDC(NULL);
						if (hMemDst)
						{
							HGDIOBJ hOldDst = SelectObject(hMemDst, hDib);
							if (hOldDst)
							{
								if (stBitmap.bmBitsPixel <= 8)
								{
									// take the DFB's palette and set it to our DIB
									HPALETTE hPalette =
										(HPALETTE)GetCurrentObject(hMemSrc, OBJ_PAL);
									if (hPalette)
									{
										PALETTEENTRY pPaletteEntries[0x100];
										UINT nEntries = GetPaletteEntries(hPalette,
											0, stBitmapInfo.bmiHeader.biClrUsed,
											pPaletteEntries);
										if (nEntries)
										{
											ASSERT(nEntries <= 0x100);
											for (UINT nIndex = 0; nIndex < nEntries; nIndex++)
												pPaletteEntries[nEntries].peFlags = 0;
											VERIFY(SetDIBColorTable(hMemDst, 0,
												nEntries, (RGBQUAD*)pPaletteEntries) == nEntries);

										}
									}
								}

								// transfer the image using BitBlt function.
								// It will probably end in the
								// call to driver's DrvCopyBits function.
								if (BitBlt(hMemDst, 0, 0, stBitmap.bmWidth,
									stBitmap.bmHeight, hMemSrc, 0, 0, SRCCOPY))
									bConverted = true; // success

								VERIFY(SelectObject(hMemDst, hOldDst));
							}
							VERIFY(DeleteDC(hMemDst));
						}
						VERIFY(SelectObject(hMemSrc, hOldSrc));
					}
					VERIFY(DeleteDC(hMemSrc));
				}

				if (bConverted)
				{
					VERIFY(DeleteObject(hBitmap)); // it's no longer needed
					hBitmap = hDib;
				}
				else
					VERIFY(DeleteObject(hDib));
			}
			ReleaseDC(NULL, hScreen);
		}
	}
	return bConverted;
}

template <class T> void VideoEncoder::SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

HBITMAP VideoEncoder::LoadImageFromFilePath(LPCWSTR filePath) {
	return (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

int VideoEncoder::GetDIBFromHandle(HBITMAP hBitmap, BITMAP *bitmap) {
	bool success = ConvertToDIB(hBitmap);
	int errorCode = GetObject(hBitmap, sizeof(BITMAP), bitmap);
	return errorCode;
}

UINT64 VideoEncoder::CalcVideoFrameDuration(VideoSettings vs) {
	return 10 * 1000 * 1000 / vs.videoFps;
}

UINT32 VideoEncoder::CalcVideoFrameCount(VideoSettings vs, int duration) {
	return duration * vs.videoFps;
}

void VideoEncoder::SetVideoSettings(VideoSettings vs) {
	mVideoBitRate = vs.videoBitRate;
	mVideoEncodingFormat = vs.videoEncodingFormat;
	mVideoFps = vs.videoFps;
}

HRESULT VideoEncoder::InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD *pAudioStreamIndex, LPCWSTR videoOutputFilePath, IMFSourceReader *pReader)
{
	*ppWriter = NULL;
	*pStreamIndex = NULL;

	IMFSinkWriter   *pSinkWriter = NULL;
	IMFMediaType  *videoTypeOut = NULL;  
	IMFMediaType  *videoTypeIn = NULL;   
	IMFMediaType  *audioTypeOut = NULL;
	IMFMediaType  *audioTypeIn = NULL;
	DWORD           streamIndex;
	DWORD audioStreamIndex;

    //create SinkWriter
	HRESULT hr = MFCreateSinkWriterFromURL(videoOutputFilePath, NULL, NULL, &pSinkWriter);

	// Set the output video type.
	if (SUCCEEDED(hr))
	{
		hr = MFCreateMediaType(&videoTypeOut);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeOut->SetGUID(MF_MT_SUBTYPE, mVideoEncodingFormat);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeOut->SetUINT32(MF_MT_AVG_BITRATE, mVideoBitRate);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeSize(videoTypeOut, MF_MT_FRAME_SIZE, mVideoWidth, mVideoHeight);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeRatio(videoTypeOut, MF_MT_FRAME_RATE, mVideoFps, 1);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeRatio(videoTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSinkWriter->AddStream(videoTypeOut, &streamIndex);
	}

	// Set the input video type.
    //the subtype is set from a property, which is set when VideoEncoder is initialized
    //most of the other type properties are set this way as well
	if (SUCCEEDED(hr))
	{
		hr = MFCreateMediaType(&videoTypeIn);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeIn->SetGUID(MF_MT_SUBTYPE, mVideoInputFormat);
	}
	if (SUCCEEDED(hr))
	{
		hr = videoTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeSize(videoTypeIn, MF_MT_FRAME_SIZE, mVideoWidth, mVideoHeight);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeRatio(videoTypeIn, MF_MT_FRAME_RATE, mVideoFps, 1);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFSetAttributeRatio(videoTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSinkWriter->SetInputMediaType(streamIndex, videoTypeIn, NULL);
	}

	//set audio type out
    //can hardcode audio type out here
	//if (SUCCEEDED(hr))
	//{
	//	hr = MFCreateMediaType(&audioTypeOut);
	//}
	//if (SUCCEEDED(hr))
	//{
	//	hr = audioTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	//}
	//if (SUCCEEDED(hr))
	//{
	//	hr = audioTypeOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
	//}
	//if (SUCCEEDED(hr))
	//{
	//	hr = audioTypeOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 320);
	//}
	//if (SUCCEEDED(hr))
	//{
	//	hr = audioTypeOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
	//}
	//if (SUCCEEDED(hr))
	//{
	//	hr = audioTypeOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
	//}
	///*if (SUCCEEDED(hr))
	//{
	//hr = pSinkWriter->AddStream(audioTypeOut, &audioStreamIndex);
	//}*/

    //with SinkWriter, you can transcode between any media types installed on the client machine by just setting a different output type
    /*this section iterates through available media types for audio type out; I'm using WMA format here*/
    /*it then sets the audio type out for the SinkWriter*/
	IMFCollection *availableTypes = NULL;
	hr = MFTranscodeGetAudioOutputAvailableTypes(MFAudioFormat_WMAudioV9, MFT_ENUM_FLAG_ALL, NULL, &availableTypes);

	DWORD count = 0;
	hr = availableTypes->GetElementCount(&count);  // Get the number of elements in the list.

	IUnknown     *pUnkAudioType = NULL;
	IMFMediaType *audioOutputType = NULL;
	for (DWORD i = 0; i < count; ++i)
	{
		hr = availableTypes->GetElement(i, &pUnkAudioType);
		hr = pUnkAudioType->QueryInterface(IID_PPV_ARGS(&audioTypeOut));

		// compare channels, sampleRate, and bitsPerSample to target numbers
		{
			GUID subtype;
			hr = audioTypeOut->GetGUID(MF_MT_SUBTYPE, &subtype);

			UINT32 avgBytesPerSecond;
			hr = audioTypeOut->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSecond);

			UINT32 bitsPerSample;
			hr = audioTypeOut->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample);

			UINT32 numChannels;
			hr = audioTypeOut->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &numChannels);

			UINT32 samPerSec;
			hr = audioTypeOut->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &samPerSec);

            //when we find the quality settings we want, we break out of the loop and set the type below (line 421)
			if (55002 == avgBytesPerSecond && 2 == numChannels && 24 == bitsPerSample && 44100 == samPerSec)// && numChannels == 2 && bitsPerSample == 16 && samPerSec = 44100)
				break;
		}

		//audioOutputType.Reset();
		//SafeRelease(&audioOutputType);
	}

	SafeRelease(&availableTypes);

	if (SUCCEEDED(hr))
	{
		hr = pSinkWriter->AddStream(audioTypeOut, &audioStreamIndex);
		SafeRelease(&audioTypeOut);
	}
    /*this section iterates through available media types for audio type out; I'm using WMA format here*/
    /*it then sets the audio type out for the SinkWriter*/

	if (SUCCEEDED(hr))
	{
		//ConfigureDecoder(pReader, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM);

		//IMFMediaType *pNativeType = NULL;
		//IMFMediaType *pType = NULL;

		//// Find the native format of the stream.
		////hr = pReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &pNativeType);

        //here we set the audio type in to whatever the audio file's audio type is
		hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &audioTypeIn);

		GUID subtype;
		hr = audioTypeIn->GetGUID(MF_MT_SUBTYPE, &subtype);

		UINT32 bitsPerSample;
		hr = audioTypeIn->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample);

		//hr = audioTypeIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 320);

		UINT32 avgBytesPerSecond;
		hr = audioTypeIn->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSecond);

		UINT32 numChannels;
		hr = audioTypeIn->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &numChannels);

		UINT32 samPerSec;
		hr = audioTypeIn->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &samPerSec);

		//IMFMediaType *audioTypeIn = NULL;  // <-- declaration from above
		// NOTE: audioReader is an IMFMediaSource used to read the audio file
		//hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &audioTypeIn);
		hr = pSinkWriter->SetInputMediaType(audioStreamIndex, audioTypeIn, NULL);
		SafeRelease(&audioTypeIn);
	}

	// Tell the sink writer to start accepting data.
	if (SUCCEEDED(hr))
	{
		hr = pSinkWriter->BeginWriting();
	}

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWriter = pSinkWriter;
		(*ppWriter)->AddRef();
		*pStreamIndex = streamIndex;
		*pAudioStreamIndex = audioStreamIndex;
	}

	SafeRelease(&pSinkWriter);
	SafeRelease(&videoTypeOut);
	SafeRelease(&videoTypeIn);
	return hr;
}

HRESULT VideoEncoder::WriteFrame(
	IMFSinkWriter *pWriter,
	DWORD streamIndex,
	const LONGLONG& rtStart,        // Time stamp.
	byte* vfb       // Receives the amount of data written.
	)
{
	IMFSample *pSample = NULL;
	IMFMediaBuffer *pBuffer = NULL;

	const LONG cbWidth = 4 * mVideoWidth;
	const DWORD cbBuffer = cbWidth * mVideoHeight;

	BYTE *pData = NULL;

	// Create a new memory buffer.
    //the buffer size is based on the width and height of the video (which is in turn based on the width and height of the image used)
	HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);

	// Lock the buffer and copy the video frame to the buffer.
	if (SUCCEEDED(hr))
	{
		hr = pBuffer->Lock(&pData, NULL, NULL);
	}
	if (SUCCEEDED(hr))
	{
		hr = MFCopyImage(
			pData,                      // Destination buffer.
			cbWidth,                    // Destination stride.
			vfb,    // First row in source image.
			cbWidth,                    // Source stride.
			cbWidth,                    // Image width in bytes.
			mVideoHeight                // Image height in pixels.
			);
	}
	if (pBuffer)
	{
		pBuffer->Unlock();
	}

	// Set the data length of the buffer.
	if (SUCCEEDED(hr))
	{
		hr = pBuffer->SetCurrentLength(cbBuffer);
	}

	// Create a media sample and add the buffer to the sample.
	if (SUCCEEDED(hr))
	{
		hr = MFCreateSample(&pSample);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSample->AddBuffer(pBuffer);
	}

	// Set the time stamp and the duration.
	if (SUCCEEDED(hr))
	{
		hr = pSample->SetSampleTime(rtStart);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSample->SetSampleDuration(mVideoFrameDuration);
	}

	// Send the sample to the Sink Writer.
	if (SUCCEEDED(hr))
	{
		hr = pWriter->WriteSample(streamIndex, pSample);
	}

	/*if (SUCCEEDED(hr)) {
	hr = WriteWaveData(pWriter, *pReader, cbMaxAudioData, pcbDataWritten);
	}*/

	SafeRelease(&pSample);
	SafeRelease(&pBuffer);
	return hr;
}

void VideoEncoder::SetDuration(UINT64 duration) {
	mDuration = duration;
}

HRESULT VideoEncoder::StartMediaFoundation() {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr))
	{
		hr = MFStartup(MF_VERSION);
		if (SUCCEEDED(hr))
		{
			return hr;
		}
	}

	return ERROR;
}

UINT64 VideoEncoder::GetVideoFrameCount() {
	return mDuration * mVideoFps;
}

UINT64 VideoEncoder::GetVideoFrameDuration(){
	return 10 * 1000 * 1000 / mVideoFps;
}

void VideoEncoder::Encode() {
    //get handle to the image from URL (is not the image data itself, but just a file handle)
	HBITMAP h = LoadImageFromFilePath(mImageFilePath);

    //this method calls ConvertToDIB; ConvertToDIB modifies the pointer directly to gather the byte array data
    //the method finishes up with a call to GetObject, which modifies the bitmap handle pointer to point to an actual bitmap object
	BITMAP b;
	GetDIBFromHandle(h, &b);

    //sets the video's width and height from the bitmap object
	SetVideoHeightAndWidth(b);

    //starts media foundation (media foundation/gdi+/etc. are required to be started [and stopped when finished] before you can use them
	HRESULT hr = StartMediaFoundation();
	if (FAILED(hr))
	{
		return;
	}

    /*This section retrieves the media's duration (in this case, the audio file)*/
	MFTIME mft;
	IMFMediaSource *ms = NULL;

	CreateMediaSource(
		mAudioFilePath,
		NULL,    // Optional, can be NULL
		&ms
		);

	hr = GetSourceDuration(ms, &mft);
	if (FAILED(hr))
	{
		return;
	}
	mDuration = mft / 10 / 1000 / 1000;

    //SafeRelease is a method Microsoft suggests using; it's merely a "safe" way to nullify and release the pointer
	SafeRelease(&ms);
    /*This section retrieves the media's duration (in this case, the audio file)*/

    //this creates a SourceReader; the SourceReader can be used to read MediaSamples directly from a source (url, stream, etc.)
	IMFSourceReader *pReader = NULL;
	hr = MFCreateSourceReaderFromURL(mAudioFilePath, NULL, &pReader);
	if (FAILED(hr))
	{
		return;
	}

	IMFSinkWriter *pSinkWriter = NULL;
	DWORD stream;
	DWORD audioStream;

    //some simple calculations to determine frame duration and total video frame count
	UINT64 videoFrameDuration = GetVideoFrameDuration();
	UINT64 videoFrameCount = GetVideoFrameCount();

    //this method initializes the SinkWriter; it creates streams, sets up media types, and calls BeginWriting
	hr = InitializeSinkWriter(&pSinkWriter, &stream, &audioStream, mVideoOutputPath, pReader);
	if (FAILED(hr))
	{
		return;
	}

	LONGLONG rtStart = 0;
	DWORD pStreamFlags;
	LONGLONG timestamp = 0;
	IMFSample *audioSample = NULL;

	/*std::cout << "Starting encoding..." << std::endl;*/

    //read the initial audio sample
	hr = pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &pStreamFlags, &timestamp, &audioSample);
	if (FAILED(hr))
	{
		return;
	}

    //while we can keep pulling audio samples (this is a NULL check)
	while (audioSample) {
        //get the duration of the audio sample
		LONGLONG *sampleDuration = new LONGLONG();
		hr = audioSample->GetSampleDuration(sampleDuration);
		if (FAILED(hr))
		{
			break;
		}

        //get the sample byte length of the buffer
		DWORD *sampleLength = new DWORD();
		hr = audioSample->GetTotalLength(sampleLength);
		if (FAILED(hr))
		{
			break;
		}

        //get a reference to the read sample's media buffer (the buffer contains the actual audio data)
		IMFMediaBuffer *pBuffer = NULL;
		hr = audioSample->GetBufferByIndex(0, &pBuffer);
		if (FAILED(hr))
		{
			break;
		}

        //determine how many video frames fit into one audio frame
        //I'm doing this to keep the frame writing in sync; originally, I was writing the audio samples as they came out, but this lead to the writer seeking in the file and substantially slowing down processing
        //TODO handle high frames per second, i.e. the video frame length is ALWAYS bigger than the audio length
        //from what I've seen through testing, the audio frames have a duration of 1000000, and the video frames at 30 FPS have a duration of 333333
		int count = *sampleDuration / (double)videoFrameDuration;
		int tempLength = (count == 0) ? *sampleLength : *sampleLength / count;

        //this gets hit when only one audio frame remains, and it's length is less than that of a video frame
		if (count == 0) 
		{
			//write audio frame
			hr = pSinkWriter->WriteSample(audioStream, audioSample);
			if (FAILED(hr))
			{
				break;
			}

			//write video frame
            //bmBits comes from the DIB bitmap object created above
			hr = WriteFrame(pSinkWriter, stream, rtStart, (byte*)b.bmBits);
			if (FAILED(hr))
			{
				break;
			}
			std::cout << "100" << std::endl;

            //rtStart keeps track of how much of the duration has been written so far
			rtStart += videoFrameDuration;
		}

        //for each video frame that fits one audio frame in duration
		for (int i = 0; i < count; i++) {

            //create a temp buffer for each split audio sample
			IMFMediaBuffer *tempBuffer = NULL;
			hr = MFCreateMemoryBuffer(tempLength, &tempBuffer);
			if (FAILED(hr))
			{
				break;
			}
            //determine the offset
			LONGLONG offset = i * tempLength;

			//write audio frame
            //use MFCreateMediaBufferWrapper to split the audio sample into i parts
			hr = MFCreateMediaBufferWrapper(
				pBuffer,
				offset,
				tempLength,
				&tempBuffer
				);
			if (FAILED(hr))
			{
				break;
			}

            //create a media sample using the temporary buffer
			IMFSample *tempSample = NULL;
			hr = MFCreateSample(&tempSample);
			if (SUCCEEDED(hr))
			{
				hr = tempSample->AddBuffer(tempBuffer);
			}
			if (SUCCEEDED(hr))
			{
				hr = tempSample->SetSampleTime(rtStart);
			}
			if (SUCCEEDED(hr))
			{
				hr = tempSample->SetSampleDuration(videoFrameDuration);
			}

            //write the audio sample to the SinkWriter
			hr = pSinkWriter->WriteSample(audioStream, tempSample);
			if (FAILED(hr))
			{
				break;
			}

			//write video frame
			hr = WriteFrame(pSinkWriter, stream, rtStart, (byte*)b.bmBits);
			if (FAILED(hr))
			{
				break;
			}
			std::cout << std::setw(2) << (int)(((double)rtStart / (double)mft) * 100) << "\r" << std::flush;

            //rtStart keeps track of how much of the duration has been written so far
			rtStart += videoFrameDuration;
			SafeRelease(&tempBuffer);

		}

        //memory cleanup
		delete sampleDuration;
		delete sampleLength;
		SafeRelease(&pBuffer);

		if (FAILED(hr))
		{
			break;
		}

        //read next sample for while loop
		hr = pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &pStreamFlags, &timestamp, &audioSample);
		if (FAILED(hr))
		{
			break;
		}
	}

    //finish and finalize
	if (SUCCEEDED(hr))
	{
		hr = pSinkWriter->Finalize();
	}
	SafeRelease(&pSinkWriter);

    //media foundation shutdown
	MFShutdown();
    //Microsoft application shutdown
	CoUninitialize();

	/*std::cout << "Encoding complete!" << std::endl;*/
}

void VideoEncoder::SetVideoHeightAndWidth(BITMAP bitmap) {
	mVideoHeight = bitmap.bmHeight;
	mVideoWidth = bitmap.bmWidth;
}

HRESULT VideoEncoder::CreateMediaSource(
	PCWSTR pszURL,
	IPropertyStore *pConfig,    // Optional, can be NULL
	IMFMediaSource **ppSource
	)
{
	IMFSourceResolver* pSourceResolver = NULL;
	IUnknown* pSource = NULL;

	// Create the source resolver.
	HRESULT hr = MFCreateSourceResolver(&pSourceResolver);

	// Use the source resolver to create the media source.
	if (SUCCEEDED(hr))
	{
		MF_OBJECT_TYPE ObjectType;

		//DbgLog(L"CreateObjectFromURL");

		hr = pSourceResolver->CreateObjectFromURL(
			pszURL,
			MF_RESOLUTION_MEDIASOURCE,  // Create a media source.
			pConfig,                    // Configuration properties.
			&ObjectType,                // Receives the object type. 
			&pSource
			);

		//DbgLog(L"CreateObjectFromURL - FINISHED");

	}

	if (SUCCEEDED(hr))
	{
		hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));
	}

	SafeRelease(&pSourceResolver);
	SafeRelease(&pSource);
	return hr;
}

HRESULT VideoEncoder::GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration)
{
	*pDuration = 0;

	IMFPresentationDescriptor *pPD = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (SUCCEEDED(hr))
	{
		hr = pPD->GetUINT64(MF_PD_DURATION, (UINT64*)pDuration);
		pPD->Release();
	}
	return hr;
}

HRESULT VideoEncoder::ConfigureDecoder(IMFSourceReader *pReader, DWORD dwStreamIndex)
{
	IMFMediaType *pNativeType = NULL;
	IMFMediaType *pType = NULL;

	// Find the native format of the stream.
	HRESULT hr = pReader->GetNativeMediaType(dwStreamIndex, 0, &pNativeType);
	if (FAILED(hr))
	{
		return hr;
	}

	GUID majorType, subtype;

	// Find the major type.
	hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
	if (FAILED(hr))
	{
		goto done;
	}

	// Define the output type.
	hr = MFCreateMediaType(&pType);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pType->SetGUID(MF_MT_MAJOR_TYPE, majorType);
	if (FAILED(hr))
	{
		goto done;
	}

	// Select a subtype.
	if (majorType == MFMediaType_Video)
	{
		subtype = MFVideoFormat_RGB32;
	}
	else if (majorType == MFMediaType_Audio)
	{
		subtype = MFAudioFormat_PCM;
	}
	else
	{
		// Unrecognized type. Skip.
		goto done;
	}

	hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);
	if (FAILED(hr))
	{
		goto done;
	}

	// Set the uncompressed format.
	hr = pReader->SetCurrentMediaType(dwStreamIndex, NULL, pType);
	if (FAILED(hr))
	{
		goto done;
	}

done:
	SafeRelease(&pNativeType);
	SafeRelease(&pType);
	return hr;
}

//HRESULT CreateMediaSample(DWORD cbData, IMFSample **ppSample)
//{
//	HRESULT hr = S_OK;
//
//	IMFSample *pSample = NULL;
//	IMFMediaBuffer *pBuffer = NULL;
//
//	hr = MFCreateSample(&pSample);
//
//	if (SUCCEEDED(hr))
//	{
//		hr = MFCreateMemoryBuffer(cbData, &pBuffer);
//	}
//
//	if (SUCCEEDED(hr))
//	{
//		hr = pSample->AddBuffer(pBuffer);
//	}
//
//	if (SUCCEEDED(hr))
//	{
//		*ppSample = pSample;
//		(*ppSample)->AddRef();
//	}
//
//	SafeRelease(&pSample);
//	SafeRelease(&pBuffer);
//	return hr;
//}

