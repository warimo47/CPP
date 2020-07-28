// Copyright (c) 2020 Kangsan Bae <warimo47@naver.com>. All rights reserved.

#include <iostream>
#include <fstream>
#include <string>

int main()
{
	std::ifstream numberFile{ "01+=+[5천개]+45+적용+=+0001.use") };
	std::string str = "";
	
	if (numberFile.is_open() == true)
	{
		while (numberFile.eof() == false)
		{

		}
	}

	

	std::getline(numberFile, str);


	std::cout << allText;

	getchar();
}