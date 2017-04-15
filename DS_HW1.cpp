#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <atomic>
#include <time.h>
#include <windows.h>
#include <iostream>
#define NUM_THREAD 8

using namespace std;
#pragma comment(lib,"pthreadVC2.lib") 

atomic <long int> sum = 0;
clock_t start, finish;
pthread_mutex_t lock;
long int Round;

//Fetch and Add
void *FAA(void *arg)
{
	for(long int i=0;i<Round;i++)
		atomic_fetch_add(&sum, 1);
	return (void *)0;
}

//Compare and Swap
void *CAS(void *arg)
{
	long cur_sum;
	for (int i = 0; i< Round; i++)
	{
		cur_sum = sum;
		bool flag = sum.compare_exchange_weak(cur_sum, cur_sum + 1);
		if (!flag)
			i = i - 1;
	}
	return (void *)0;
}

//Mutex Lock
void *ML(void *arg)
{
	for (long int i = 0; i < Round; i++)
	{
		pthread_mutex_lock(&lock);
		atomic_fetch_add(&sum, 1);
		pthread_mutex_unlock(&lock);
	}
	return (void *)0;
}

//Change character's color
void SetColor(unsigned short ForeColor, unsigned short BackGroundColor)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, (ForeColor % 16) | (BackGroundColor % 16 * 16));
}

int main()
{
	int num;
	pthread_t threads[NUM_THREAD];
	int ret, t;
	double runtime;

	Round = 1000000000 / NUM_THREAD;
	pthread_mutex_init(&lock, NULL);

	while (1)
	{
		cout << "1. Fetch and Add" << endl;
		cout << "2. Compare and Swap" << endl;
		cout << "3. Mutex Lock" << endl;

		cout << "Choose synchronization operation (Other num exit):";
		cin >> num;
		cout << "---------------------------------------------------" << endl;

		if (num == 1)
			cout << "【Fetch and Add】--Thread Num: " << NUM_THREAD << endl;
	
		else if (num == 2)
			cout << "【Compare and Swap】--Thread Num: " << NUM_THREAD << endl;

		else if (num == 3)
			cout << "【Mutex Lock】--Thread Num: " << NUM_THREAD << endl;

		else
			break;

		start = clock();     //start time

		/* pthread_create */
		for (t = 0; t < NUM_THREAD; t++)
		{
			if (num == 1)
				ret = pthread_create(&threads[t], NULL, FAA, NULL);
			else if (num == 2)
				ret = pthread_create(&threads[t], NULL, CAS, NULL);
			else if (num == 3)
				ret = pthread_create(&threads[t], NULL, ML, NULL);

			if (ret) {
				cout << " create thread error " << endl;
				return -1;
			}
		}
		/* pthread_join */
		for (t = 0; t < NUM_THREAD; t++)
			ret = pthread_join(threads[t], NULL);

		finish = clock();     //finish time

		runtime = double(finish - start) / CLOCKS_PER_SEC;

		SetColor(14, 0);
		cout << "sum is " << sum << ",runtime is " << runtime << " s" << endl << endl;
		SetColor(7, 0);
		sum = 0;
	}
	return 0;
}
