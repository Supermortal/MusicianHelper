#include "stdafx.h"
#include "VideoEncoder.h"

#include <vcclr.h>
#using <System.dll>

using namespace System;

namespace SupermortalNative{
    public ref class VideoEncoderWrapper
    {
    public:
        VideoEncoderWrapper() : m_Impl(new VideoEncoder) {}
		VideoEncoderWrapper(LPCWSTR imageFilePath, LPCWSTR audioFilePath, LPCWSTR videoOutputPath) : m_Impl(new VideoEncoder(imageFilePath, audioFilePath, videoOutputPath)) {}
        ~VideoEncoderWrapper() {
            delete m_Impl;
        }
        void Encode() {
            m_Impl->Encode();
        }
				void SetImageFilePath(System::String ^ imageFilePath) {
					m_Impl->SetImageFilePath(imageFilePath);
				}
				void SetVideoOutputPath(System::String ^ videoOutputPath){
					m_Impl->SetVideoOutputPath(videoOutputPath);
				}
				void SetAudioFilePath(System::String ^ audioFilePath){
					m_Impl->SetAudioFilePath(audioFilePath);
				}
    protected:
        !VideoEncoderWrapper() {
            delete m_Impl;
        }
    private:
        VideoEncoder *m_Impl;
    };
}
