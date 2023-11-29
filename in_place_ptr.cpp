#include <map>
#include <string>
#include <vector>
#include <memory>
#include "in_place_ptr.h"
#include <Windows.h>
#include <WinBase.h>

struct Fruit
{
	virtual void foo()
	{
		printf("Fruit");
	}
	//	double x, y, z;
	double matrix[4][4];
};

struct Apple : public Fruit
{
	virtual void foo()
	{
		printf("Apple");
	}
};

struct Orange : public Fruit
{
	virtual void foo()
	{
		printf("Orange");
	}
	//	double	extra_data;
};

template<typename V>
void testHolder()
{
	V	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.push_back(Apple());
		vA.push_back(Orange());
	}
	vA.clear();
}

template<typename V>
void testHolderS()
{
	V	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.push_back(Fruit());
		vA.push_back(Fruit());
	}
	vA.clear();
}

template<typename V>
void testHolderSE()
{
	V	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.emplace_back(Fruit());
		vA.emplace_back(Fruit());
	}
	vA.clear();
}

template<typename V>
void testHolderE()
{
	V	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.emplace_back(Apple());
		vA.emplace_back(Orange());
	}
	vA.clear();
}

void testPtr()
{
	std::vector<std::shared_ptr<Fruit>>	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.push_back(std::make_shared<Apple>());
		vA.push_back(std::make_shared<Orange>());
	}
	vA.clear();
}

void testUniquePtr()
{
	std::vector<std::unique_ptr<Fruit>>	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.push_back(std::make_unique<Apple>());
		vA.push_back(std::make_unique<Orange>());
	}
	vA.clear();
}

void testPtrE()
{
	std::vector<std::shared_ptr<Fruit>>	vA;

	vA.reserve(1000000);
	for (size_t i = 0; i < 500000; i++)
	{
		vA.emplace_back(std::make_shared<Apple>());
		vA.emplace_back(std::make_shared<Orange>());
	}
	vA.clear();
}

typedef	std::map<std::string, size_t>	TestScore;
typedef	std::map<size_t, std::string>	TestScoreX;

void test(TestScore& ts)
{
	LARGE_INTEGER	t1, t2;

	QueryPerformanceCounter(&t1);
	testHolder<std::vector<Fruit>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime sliced %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolder<std::vector<Fruit>>(); NOTE: Object Slicing!"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolderE<std::vector<Fruit>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime slicEd %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolderE<std::vector<Fruit>>(); NOTE: Object Slicing!"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolderS<std::vector<Fruit>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime Sliced %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolderS<std::vector<Fruit>>(); NOTE: Object Slicing!"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolderSE<std::vector<Fruit>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime SlicEd %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolderSE<std::vector<Fruit>>(); NOTE: Object Slicing!"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolder<std::vector<in_place_ptr_copy_fn<Fruit, sizeof(Fruit)>>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime safe %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolder<std::vector<in_place_ptr_copy_fn<Fruit, sizeof(Fruit)>>>();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolderE<std::vector<in_place_ptr_copy_fn<Fruit, sizeof(Fruit)>>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime safE %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolderE<std::vector<in_place_ptr_copy_fn<Fruit, sizeof(Fruit)>>>();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolder<std::vector<in_place_ptr_move_copy<Fruit, sizeof(Fruit)>>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime fncp %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolder<std::vector<in_place_ptr_move_copy<Fruit, sizeof(Fruit)>>>();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolder<std::vector<in_place_ptr_memcpy<Fruit, sizeof(Fruit)>>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime memcp %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolder<std::vector<in_place_ptr_memcpy<Fruit, sizeof(Fruit)>>>();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testHolderE<std::vector<in_place_ptr_memcpy<Fruit, sizeof(Fruit)>>>();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime memcE %ld", t2.QuadPart - t1.QuadPart);
	ts["testHolderE<std::vector<in_place_ptr_memcpy<Fruit, sizeof(Fruit)>>>();"] += t2.QuadPart - t1.QuadPart;


	QueryPerformanceCounter(&t1);
	testPtr();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime std %ld", t2.QuadPart - t1.QuadPart);
	ts["testPtr();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testUniquePtr();
	QueryPerformanceCounter(&t2);
	//	printf("\nTime std %ld", t2.QuadPart - t1.QuadPart);
	ts["testUniquePtr();"] += t2.QuadPart - t1.QuadPart;

	QueryPerformanceCounter(&t1);
	testPtrE();
	QueryPerformanceCounter(&t2);
	//	printf("\nTimE std %ld", t2.QuadPart - t1.QuadPart);
	ts["testPtrE();"] += t2.QuadPart - t1.QuadPart;

}

int main(int argc, char* argv[])
{
	const int count = 100;
	printf("\nAbbreviations:\n S - single (base) class\n E - emplace_back rather than push_back\n");
	printf("\nRunning (%d) ", count);

	TestScore	ts;
	for (int i = 0; i < count; ++i)
	{
		test(ts);
		printf(".");
	}
	printf("\n");

	TestScoreX	tsx;

	for (auto x : ts)
		tsx[x.second] = x.first;


	for (auto x : tsx)
		printf("\n%zd, %s", x.first, x.second.c_str());

	printf("\n");
	//#ifdef _DEBUG
	getchar();
	//#endif
	return 0;
}

