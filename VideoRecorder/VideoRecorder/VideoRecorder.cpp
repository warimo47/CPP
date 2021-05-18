// VideoRecorder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

#include <iostream>
#include <locale>
#include <codecvt>
#include <thread>

#include "opencv2/opencv.hpp"
#include "Windows.h"

void VideoRecording(cv::String cvsVSA, cv::String cvsVFN);

int main()
{
    wchar_t excuteProgramPath[256] = { 0, };
	GetModuleFileName(NULL, excuteProgramPath, sizeof(excuteProgramPath) / 2);
	std::wstring excuteFolderPath = excuteProgramPath;
	std::wstring::size_type stTmp;
	stTmp = excuteFolderPath.find(L"VideoRecorder.exe", 0);
	excuteFolderPath.erase(stTmp, 17);
	std::wstring iniFilePath = excuteFolderPath + L"init.ini";

	cv::String cvsVideoStreamingAddresses[12];
	wchar_t wcaReturn[1024] = { 0, };
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	
	GetPrivateProfileStringW(L"Video streaming path", L"0", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[0] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"1", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[1] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"2", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[2] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"3", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[3] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"4", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[4] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"5", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[5] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"6", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[6] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"7", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[7] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"8", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[8] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"9", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[9] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"10", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[10] = converterX.to_bytes(wcaReturn);

	GetPrivateProfileStringW(L"Video streaming path", L"11", L"", wcaReturn, 1024, iniFilePath.c_str());
	cvsVideoStreamingAddresses[11] = converterX.to_bytes(wcaReturn);

	std::thread* threads[12] = { nullptr, };
	std::string cvsVideoFileName;
	for (int i = 0; i < 12; ++i)
	{
		cvsVideoFileName = converterX.to_bytes(excuteFolderPath + std::to_wstring(i) + L".mp4");
		threads[i] = new std::thread(VideoRecording, cvsVideoStreamingAddresses[i], cvsVideoFileName);
	}

	for (int i = 0; i < 12; ++i)
	{
		threads[i]->join();
	}
}

void VideoRecording(cv::String cvsVSA, cv::String cvsVFN)
{
	cv::VideoCapture* videoCapture = nullptr;
	cv::VideoWriter* videoWriters = nullptr;
	cv::Mat* mats = nullptr;

	float videoFPS = 0.f;
	int videoWidth = 0;
	int videoHeight = 0;

	bool re = false;
	videoCapture = new cv::VideoCapture(cvsVSA, cv::CAP_FFMPEG);

	videoFPS = videoCapture->get(cv::CAP_PROP_FPS);
	videoWidth = videoCapture->get(cv::CAP_PROP_FRAME_WIDTH);
	videoHeight = videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT);

	videoWriters = new cv::VideoWriter;

	re = videoWriters->open(
		cvsVFN,
		cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
		videoFPS,
		cv::Size(videoWidth, videoHeight),
		true);

	mats = new cv::Mat();

	for (int count = 0; count < videoFPS * 10; ++count)
	{
		*videoCapture >> *mats;
		*videoWriters << *mats;
	}

	videoCapture->release();
	videoCapture = nullptr;

	videoWriters->release();
	videoWriters = nullptr;

	mats->release();
	mats = nullptr;
}