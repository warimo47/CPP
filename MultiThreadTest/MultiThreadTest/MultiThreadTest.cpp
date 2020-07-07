#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// std::mutex mylock;
int sum = 0;

void threadFunction()
{
	for (auto i = 0; i < 25000000; ++i)
	{
		// mylock.lock();
		sum += 2;
		// mylock.unlock();
	}
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();

	std::thread thread1{ threadFunction };
	std::thread thread2{ threadFunction };

	thread1.join();
	thread2.join();

	auto end = std::chrono::high_resolution_clock::now() - start;

	std::cout << "Sum = " << sum << "\tTime = " << std::chrono::duration_cast<std::chrono::milliseconds>(end).count() << "msecs\n";

	getchar();
}
