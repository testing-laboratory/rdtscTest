#include <atomic>
#include <cstdint>
#include <iostream>

#include <intrin.h>

std::atomic<uint64_t> g_time = ATOMIC_VAR_INIT(0);

#define RDTSCP(CoreIdRef) __rdtscp(CoreIdRef)
//#define RDTSCP(CoreIdRef) __rdtsc()

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

uint64_t readTSCtoMem(uint64_t cnt)
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
	return 0;
}

uint64_t readMemory(uint64_t cnt)
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
	return 0;
}

uint64_t writeToMemory(uint64_t cnt)
{
	constexpr static const std::memory_order order = std::memory_order_relaxed;
	uint64_t value = 1;
	for (uint64_t x = cnt / 16; x != 0; --x)
	{
#define WRITE_MEMORY() g_time.store(value++, order);
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
		WRITE_MEMORY();
	}
	return 0;
}

uint64_t readMemoryWithChecks(uint64_t cnt)
{
	constexpr static const std::memory_order order = std::memory_order_relaxed;
	uint64_t v = 1; // special variable to prevent compiler from removing rdtsc calls
	unsigned int coreId = 0;
	uint64_t old = 0, count = 0, next;
	for (uint64_t x = cnt / 16; x != 0; --x)
	{
#define READ_WITH_CHECKS() next = g_time.load(order); if (next == old) { ++count; } else { old = next; } v += __rdtsc(); v += __rdtsc(); // v += RDTSCP(&coreId);
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
		READ_WITH_CHECKS();
	}
	return v == 0 ? 666 : count;
}

struct Timer
{
	Timer();
	uint64_t GetMilliseconds() const;
private:
	uint64_t freq;
	uint64_t start;
};

void PrintDelimeter()
{
	std::cout << "=====================================================================" << std::endl;
}

typedef uint64_t(*ExperimentFunc)(uint64_t count);
void DoExperiment(const char* name, ExperimentFunc proc, uint64_t cnt);

#include <thread>

int main()
{
	std::cout << "HELLO" << std::endl;

	const uint64_t cntReadTSC = 100 * 1000 * 1000ULL;
	const uint64_t cntReadMemory = 10 * 1000 * 1000 * 1000ULL;

#if 0
	DoExperiment("readTSC", readTSC, cntReadTSC * 2);
	DoExperiment("readTSCtoMem", readTSCtoMem, cntReadTSC);
	DoExperiment("readMemory", readMemory, cntReadMemory);
	DoExperiment("writeToMemory", writeToMemory, cntReadTSC * 100);
	DoExperiment("readMemoryWithChecks", readMemoryWithChecks, cntReadMemory);
#endif

#if 0
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

#if 1
	PrintDelimeter();
	std::cout << "DO THE SAME IN PARALLEL; Calculate correct reading memory..." << std::endl;
	PrintDelimeter();

	{
		std::thread t1([cntReadTSC]() { DoExperiment("writeToMemory", writeToMemory, cntReadTSC * 200); });
		std::thread t2([cntReadMemory]() { DoExperiment("readMemoryWithChecks", readMemoryWithChecks, cntReadMemory / 100); });
		t1.join();
		t2.join();
	}
#endif

	PrintDelimeter();
	std::cout << "END" << std::endl;
	return 0;
}

#include <mutex>
std::mutex g_protectConsole;

void DoExperiment(const char* name, ExperimentFunc proc, uint64_t cnt)
{
	{
		std::lock_guard<std::mutex> lock(g_protectConsole);
		PrintDelimeter();
		std::cout << "Begin of experiment " << name << "(" << double(cnt) << ")..." << std::endl;
	}
	Timer time;
	const uint64_t res = proc(cnt);
	const uint64_t ms = time.GetMilliseconds();
	{
		std::lock_guard<std::mutex> lock(g_protectConsole);
		std::cout << "End of experiment " << name << "(" << double(cnt) << ")" << std::endl;
		std::cout << name << " result value: " << res << "; percentile: " << (res * 10000 / cnt) << std::endl;
		std::cout << name << " time taken: totally: " << ms << "ms; average cycle time: " << (ms * 1000 * 1000 / cnt) << "ns or " << (ms * 1000 * 1000 * 1000 / cnt) << "ps" << std::endl;
	}
}

#include <windows.h>

Timer::Timer()
{
	static_assert(sizeof(LARGE_INTEGER) == sizeof(freq), "bad type chosen!");
	static_assert(sizeof(LARGE_INTEGER) == sizeof(start), "bad type chosen!");
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
}

uint64_t Timer::GetMilliseconds() const
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	return (end.QuadPart - start) * 1000 / freq;
}
