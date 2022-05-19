#pragma once

#include <iostream>
#include <vector>

using namespace std;

void BinarySearch(int N)
{
    vector<int> numbers = vector<int>{ 1, 8, 15, 23, 44, 56, 63, 81, 91 };

    int left = 0;
    int right = (int)numbers.size() - 1;

    while (left <= right)
    {
        cout << "탐색 범위: " << left << " - " << right << endl;

        int mid = (left + right) / 2;

        if (N < numbers[mid])
        {
            cout << N << " < " << numbers[mid] << endl;
            right = mid - 1;
        }
        else if (N > numbers[mid])
        {
            cout << N << " > " << numbers[mid] << endl;
            left = mid + 1;
        }
        else
        {
            cout << "찾음!" << endl;
            break;
        }
    }
}
