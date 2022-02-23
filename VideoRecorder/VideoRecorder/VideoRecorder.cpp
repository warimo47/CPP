// VideoRecorder.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

#include <iostream>
#include <locale>
#include <codecvt>
#include <thread>
#include <atomic>
#include <string>

#include "opencv2/opencv.hpp"
#include "Windows.h"

void VideoRecording(std::string _sVSA, std::string _sVFN, int _videoLengthSec, int _ICF);

std::atomic<bool> isRecording = false;

int main()
{
	int videoLengthSec = 3600;
    wchar_t excuteProgramPath[256] = { 0, };
	GetModuleFileName(NULL, excuteProgramPath, sizeof(excuteProgramPath) / 2);
	std::wstring excuteFolderPath = excuteProgramPath;
	std::wstring::size_type stTmp;
	stTmp = excuteFolderPath.find(L"VideoRecorder.exe", 0);
	excuteFolderPath.erase(stTmp, 17);
	std::wstring iniFilePath = excuteFolderPath + L"init.ini";

	std::string sVideoStreamingAddresses;
	wchar_t wcaReturn[1024] = { 0, };
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	
	GetPrivateProfileStringW(L"Video streaming path", L"0", L"", wcaReturn, 1024, iniFilePath.c_str());
	sVideoStreamingAddresses = converterX.to_bytes(wcaReturn);
	std::string sExcuteFolderPath;
	sExcuteFolderPath.assign(excuteFolderPath.begin(), excuteFolderPath.end());
	
	std::string subVSA = sVideoStreamingAddresses.substr(4);
	if (subVSA.compare("rtsp") != 0)
	{
		// sVideoStreamingAddresses = sExcuteFolderPath + sVideoStreamingAddresses;
	}

	GetPrivateProfileStringW(L"VideoLength", L"sec", L"", wcaReturn, 1024, iniFilePath.c_str());
	// std::wstring wsVideoSecs = wcaReturn;
	videoLengthSec = _wtoi(wcaReturn);

	GetPrivateProfileStringW(L"ImageCaptureFrame", L"frame", L"", wcaReturn, 1024, iniFilePath.c_str());
	int imageCaptureFrame = _wtoi(wcaReturn);

	std::thread* threads = nullptr;
	std::string sSaveVideoFolder;

	sSaveVideoFolder.append(excuteFolderPath.begin(), excuteFolderPath.end());
	isRecording = true;
	threads = new std::thread(VideoRecording, sVideoStreamingAddresses, sSaveVideoFolder, videoLengthSec, imageCaptureFrame);

	int keyCommand = 0;

	while (true)
	{
		if (isRecording == false) break;
		std::cin >> keyCommand;

		if (keyCommand == 1)
		{
			isRecording = false;
			break;
		}
	}

	threads->join();

	std::cout << "프로그램 종료\n";
}

void VideoRecording(std::string _sVSA, std::string _sVFN, int _videoLengthSec, int _ICF)
{
	cv::VideoCapture* videoCapture = nullptr;
	cv::VideoWriter* videoWriters = nullptr;
	cv::Mat* mats = nullptr;

	float videoFPSf = 0.f;
	int videoFPSd = 0;
	int videoWidth = 0;
	int videoHeight = 0;

	bool re = false;

	std::cout << "비디오 녹화 시작\n";

	videoCapture = new cv::VideoCapture(_sVSA, cv::CAP_FFMPEG);

	videoFPSf = videoCapture->get(cv::CAP_PROP_FPS);
	videoFPSd = static_cast<int>(videoFPSf + 0.5f);
	videoWidth = videoCapture->get(cv::CAP_PROP_FRAME_WIDTH);
	videoHeight = videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT);

	videoWriters = new cv::VideoWriter;

	re = videoWriters->open(
		_sVFN + "0.mp4",
		cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
		static_cast<double>(_ICF),
		cv::Size(videoWidth, videoHeight),
		true);

	mats = new cv::Mat();

	int frameCount = 0;
	int fileCount = 1;

	while (true)
	{
		if (isRecording == false) break;
		if ((frameCount != 0) && ((frameCount % (videoFPSd * 60 * 60)) == 0))
		{
			std::cout << frameCount / (videoFPSd * 60 * 60) << "시간 녹화되었습니다.\n";
		}
		
		*videoCapture >> *mats;

		if (frameCount % (videoFPSd / _ICF) == 0)
		{
			*videoWriters << *mats;
		}

		if ((frameCount != 0) && (frameCount % (videoFPSd * _videoLengthSec) == 0))
		{
			videoWriters->release();

			re = videoWriters->open(
				_sVFN + std::to_string(fileCount) + ".mp4",
				cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
				static_cast<float>(videoFPSd),
				cv::Size(videoWidth, videoHeight),
				true);

			fileCount++;

			*videoWriters << *mats;
		}

		frameCount++;
	}

	videoCapture->release();
	videoCapture = nullptr;

	videoWriters->release();
	videoWriters = nullptr;

	mats->release();
	mats = nullptr;

	isRecording = false;
	std::cout << "비디오 녹화 종료\n";
}