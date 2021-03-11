// ChangeClasses.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

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
    std::string oldFolder = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolov3Train/DataSet/Coco_Train_Truck_6126/";
    std::string newFolder = "C:/Users/warim/Desktop/YOLO_TRAIN/Yolov3Train/DataSet/Coco_Train_Truck_6126_ChangeClassNum/";

    std::vector<std::string> fileNames = get_files_inDirectory(oldFolder, "*.txt");

    std::ifstream txtFileReader;
    std::ofstream txtFileWriter;

    char readLine[50];
    std::string readLineStr;

    char* context = nullptr;

    char* classNum;
    std::string classNumStr;

    char changeClassLine[50];

    std::string writeLine;

    for (auto fn : fileNames)
    {
        // std::cout << oldFolder + fn << "\n";
        txtFileReader.open(oldFolder + fn, std::ios_base::in);
        txtFileWriter.open(newFolder + fn, std::ios_base::out);

        while (txtFileReader.peek() != EOF)
        {
            std::getline(txtFileReader, readLineStr);

            strcpy_s(readLine, readLineStr.c_str());
            classNum = strtok_s(readLine, " ", &context);
            classNumStr = classNum;

            // std::cout << classNumStr << "\n";
            /*
            if (classNumStr == "1") // person
            {
                writeLine = "6" + readLineStr.substr(1) + "\n"; // People
                txtFileWriter.write(writeLine.c_str(), writeLine.length());
            }
            else if (classNumStr == "3") // car
            {
                writeLine = "4" + readLineStr.substr(1) + "\n"; // Vehicle
                txtFileWriter.write(writeLine.c_str(), writeLine.length());
            }
            else if (classNumStr == "6") // bus
            {
                writeLine = "4" + readLineStr.substr(1) + "\n"; // Vehicle
                txtFileWriter.write(writeLine.c_str(), writeLine.length());
            }
            else*/ if (classNumStr == "8") // truck
            {
                writeLine = "11" + readLineStr.substr(1) + "\n"; // Ex_truck
                txtFileWriter.write(writeLine.c_str(), writeLine.length());
            }
            else
            {

            }
        }

        txtFileReader.close();
        txtFileWriter.close();

        // std::cout << fn << "\n";
    };

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