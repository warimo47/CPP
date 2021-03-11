// TurckImageCopy.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <io.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;

std::vector<std::string> get_files_inDirectory(const std::string&, const std::string&);

int main()
{
    std::string oldFolderStr = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolov3Train/DataSet/CocoAll/";
    std::string newFolderStr = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolov3Train/DataSet/CocoTruck/";

    std::vector<std::string> fileNames = get_files_inDirectory(oldFolderStr, "*.txt");

    std::ifstream in;
    std::string readLineStr;

    char readLine[50];
    char* context = nullptr;
    char* classNum;
    std::string classNumStr;

    bool haveToCopy = false;

    char txtFileName[50];
    char* fileNameWithoutExtention;
    std::string jpgFileNameStr;

    for (auto fn : fileNames)
    {
        in.open(oldFolderStr + fn);

        haveToCopy = false;

        while (in.peek() != EOF)
        {
            std::getline(in, readLineStr);

            strcpy_s(readLine, readLineStr.c_str());
            classNum = strtok_s(readLine, " ", &context);

            classNumStr = classNum;

            if (classNumStr == "8")
            {
                haveToCopy = true;
                break;
            }
        }

        in.close();

        if (haveToCopy == true)
        {
            fs::rename(oldFolderStr + fn, newFolderStr + fn);

            strcpy_s(txtFileName, fn.c_str());
            fileNameWithoutExtention = strtok_s(txtFileName, ".", &context);
            jpgFileNameStr = fileNameWithoutExtention + std::string(".jpg");

            fs::rename(oldFolderStr + jpgFileNameStr, newFolderStr + jpgFileNameStr);
        }
    }

    std::cout << "Done.\n";
}

// find all matched file in specified directory
// [INPUT]
//   >> const std::string& _path   Search path   ex) c:/directory/
//   >> const std::string& _filter Search filter ex) *.exe or *.*
// [RETURN]
//   >> std::vector<std::string>   All matched file name & extension
std::vector<std::string> get_files_inDirectory(const std::string& _path, const std::string& _filter)
{
    std::string searching = _path + _filter;

    std::vector<std::string> return_;

    _finddata_t fd;
    long handle = _findfirst(searching.c_str(), &fd);  // 현재 폴더 내 모든 파일을 찾는다.

    if (handle == -1) return return_;

    int result = 0;
    do
    {
        return_.push_back(fd.name);
        result = _findnext(handle, &fd);
    } while (result != -1);

    _findclose(handle);

    return return_;
}