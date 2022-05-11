#pragma once

template<typename T>
class Vector
{
public:
    Vector()
    {

    }

    ~Vector()
    {
        if (_data)
            delete[] _data;
    }

    void push_back(const T& value)
    {
        if (_size == _capacity)
        {
            //  증설 작업
            int newCapacity = static_cast<int>(_capacity * 1.5);
            if (newCapacity == _capacity)
                newCapacity++;

            reserve(newCapacity);
        }

        // 데이터 저장
        _data[_size] = value;

        // 데이터 개수 증가
        _size++;
    }

    void reserve(int capacity)
    {
        if (_capacity >= capacity)
            return;

        _capacity = capacity;

        T* newData = new T[_capacity];

        // 데이터 복사
        for (int i = 0; i < _size; ++i)
            newData[i] = _data[i];

        if (_data)
            delete[] _data;

        // 교체
        _data = newData;
    }

    T& operator[](const int pos) { return _data[pos]; }

    int size() { return _size; }
    int capacity() { return _capacity; }

    void clear()
    {
        if (_data)
        {
            delete[] _data;
            _data = new T[_capacity];
        }

        _size = 0;
    }

    void resize(const int newSize)
    {
        T* newData = new T[_capacity];

        _size = newSize;

        // 데이터 복사
        for (int i = 0; i < _size; ++i)
            newData[i] = _data[i];

        if (_data)
            delete[] _data;

        // 교체
        _data = newData;
    }

private:
    T* _data = nullptr;
    int     _size = 0;
    int     _capacity = 0;
};