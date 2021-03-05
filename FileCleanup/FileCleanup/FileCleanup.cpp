﻿// FileCleanup.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

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
    std::string oldFolder = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolo_mark-master/x64/Release/data/img/";
    std::string newFolder = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolo_mark-master/x64/Release/data/newimg/";

    std::vector<std::string> fileNames = get_files_inDirectory(oldFolder, "*.txt");

    std::ifstream txtFileReader;

    char oneRectangleLine[50];
    std::string oneRectangleLineStr;

    char* context = nullptr;
    
    char* classNum;
    std::string classNumStr;
    
    bool haveToCopy = false;

    char txtFileName[30];
    std::string imgFileNameStr;

    char* fileNameOnly;

    for (auto fn : fileNames)
    {
        // std::cout << oldFolder + fn << "\n";
        txtFileReader.open(oldFolder + fn, std::ios_base::in);

        haveToCopy = false;
        while (txtFileReader.peek() != EOF)
        {
            std::getline(txtFileReader, oneRectangleLineStr);
            strcpy_s(oneRectangleLine, oneRectangleLineStr.c_str());
            classNum = strtok_s(oneRectangleLine, " ", &context);
            classNumStr = classNum;

            // std::cout << classNumStr << "\n";
            // 1:person 3:car 6:bus 8:truck
            if (classNumStr == "1" || classNumStr == "3" || classNumStr == "6" || classNumStr == "8")
            {
                haveToCopy = true;
                break;
            }
        }
        txtFileReader.close();

        if (haveToCopy == true)
        {
            strcpy_s(txtFileName, fn.c_str());
            fileNameOnly = strtok_s(txtFileName, ".", &context);
            imgFileNameStr = fileNameOnly;
            imgFileNameStr += ".jpg";

            fs::copy(oldFolder + imgFileNameStr, newFolder + imgFileNameStr);
            fs::copy(oldFolder + fn, newFolder + fn);
        }

        std::cout << "\n";
        // std::cout << fn << "\n";
    };
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