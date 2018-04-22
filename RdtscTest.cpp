#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include <windows.h>
#include <intrin.h>

std::atomic<uint64_t> g_time = ATOMIC_VAR_INIT(0);
std::mutex g_protectConsole;

//#define RDTSCP(CoreIdRef) __rdtscp(CoreIdRef)
#define RDTSCP(CoreIdRef) __rdtsc()

uint64_t readTSC(uint64_t cnt)
{
	uint64_t v = 0; // special variable to prevent compiler from removing rdtsc calls
	unsigned int coreId = 0;
	for (uint64_t x = cnt / 16; x != 0; --x)
	{
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
		v += RDTSCP(&coreId);
	}
	return v;
}

void readTSCtoMem(uint64_t cnt)
{
	constexpr static const std::memory_order order = std::memory_order_release;
	unsigned int coreId = 0;
	for (uint64_t x = cnt / 16; x != 0; --x)
	{
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
		g_time.store(RDTSCP(&coreId), order);
	}
}

void readMemory(uint64_t cnt)
{
	constexpr static const std::memory_order order = std::memory_order_consume;
	for (uint64_t x = cnt / 16; x != 0; --x)
	{
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
		g_time.load(order);
	}
}

struct Timer
{
	Timer()
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);
	}

	uint64_t GetMilliseconds() const
	{
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);
		return (end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
	}

	LARGE_INTEGER freq;
	LARGE_INTEGER start;
};

void PrintDelimeter()
{
	std::cout << "=====================================================================" << std::endl;
}

void DoExperiment(const char* name, std::function<void(uint64_t cnt)> proc, uint64_t cnt)
{
	{
		std::lock_guard<std::mutex> lock(g_protectConsole);
		PrintDelimeter();
		std::cout << "Begin of experiment " << name << "(" << double(cnt) << ")..." << std::endl;
	}
	Timer time;
	proc(cnt);
	const uint64_t ms = time.GetMilliseconds();
	{
		std::lock_guard<std::mutex> lock(g_protectConsole);
		std::cout << "End of experiment " << name << "(" << double(cnt) << ")" << std::endl;
		std::cout << name << " time taken: totally: " << ms << "ms; average cycle time: " << (ms * 1000 * 1000 / cnt) << "ns or " << (ms * 1000 * 1000 * 1000 / cnt) << "ps" << std::endl;
	}
}

int main()
{
	std::cout << "HELLO" << std::endl;

	const uint64_t cntReadTSC = 100 * 1000 * 1000ULL;
	const uint64_t cntReadMemory = 10 * 1000 * 1000 * 1000ULL;

	DoExperiment("readTSC", readTSC, cntReadTSC * 2);
	DoExperiment("readTSCtoMem", readTSCtoMem, cntReadTSC);
	DoExperiment("readMemory", readMemory, cntReadMemory);

#if 1
	PrintDelimeter();
	std::cout << "DO THE SAME IN PARALLEL; Reading TSC is longer than Memory (i.e. Memory time has meaning)..." << std::endl;
	PrintDelimeter();

	{
		std::thread t1([cntReadTSC]() { DoExperiment("readTSCtoMem", readTSCtoMem, cntReadTSC * 5); });
		std::thread t2([cntReadMemory]() { DoExperiment("readMemory", readMemory, cntReadMemory / 10); });
		t1.join();
		t2.join();
	}

	PrintDelimeter();
	std::cout << "DO THE SAME IN PARALLEL; Reading Memory is longer than TSC (i.e. TSC time has meaning)..." << std::endl;
	PrintDelimeter();

	{
		std::thread t2([cntReadMemory]() { DoExperiment("readMemory", readMemory, 2 * cntReadMemory); });
		std::thread t1([cntReadTSC]() { DoExperiment("readTSCtoMem", readTSCtoMem, cntReadTSC); });
		t1.join();
		t2.join();
	}
#endif

	PrintDelimeter();
	std::cout << "END" << std::endl;
	return 0;
}
