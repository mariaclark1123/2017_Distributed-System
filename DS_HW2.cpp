#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <atomic>
#include <time.h>
#include <windows.h>
#include <iostream>
#define NUM_THREAD 4

using namespace std;
#pragma comment(lib,"pthreadVC2.lib") 

atomic <long int> ticketnumber;
atomic <long int> turn;

atomic <long int> sum = 0;
//atomic_flag lock = ATOMIC_FLAG_INIT;
atomic <bool> lock(false);
atomic <bool> lock_stream = ATOMIC_FLAG_INIT;
clock_t start, finish;
long int Round;
bool lock1 = false;

//Test and Set
void *TAS(void *arg)
{
	/*for (long int i = 0; i < Round; i++)
	{
		while(lock.test_and_set())
		{
		}
		sum++;
		lock.clear();
	}*/
	for (long int i = 0; i < Round; i++)
	{
		while (lock.exchange(true))
		{
		}
		sum++;
		lock = false;
	}
	return (void *)0;
}

//Test and Test and Set
void *TTAS(void *arg)
{
	for (long int i = 0; i < Round; i++)
	{
		while (lock_stream)
		{
		}
		while (lock_stream.exchange(true))
		{
			
		}
		sum++;
		lock_stream = false;
	}
	return (void *)0;
}

//Fetch and Add
void *FAA(void *arg)
{
	long int myturn;
	for (long int i = 0; i < Round; i++)
	{
		myturn = atomic_fetch_add(&ticketnumber, 1);
		while (myturn != turn)
		{
		}
		sum++;
		atomic_fetch_add(&turn, 1);
	}
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
	long int i;
	Round = 100000000 / NUM_THREAD;
	/*for (i = 1; i < 9; i++)
	{
		pthread_t threads[i];
		Round = 100000000 / i;

	}*/

	while (1)
	{
		ticketnumber = 0;      //Fetch and Add lock
		turn = 0;

		cout << "1.Test and Set" << endl;
		cout << "2.Test and Test and Set" << endl;
		cout << "3. Fetch and Add" << endl;
		cout << "4. Compare and Swap" << endl;

		cout << "Choose synchronization operation (Other num exit):";
		cin >> num;
		cout << "---------------------------------------------------" << endl;
		
        if (num == 1)
			cout << "ーTest and Set―--Thread Num: " << NUM_THREAD << endl;
		
		else if (num == 2)
			cout << "ーTest and Test and Set―--Thread Num: " << NUM_THREAD << endl;
		
		else if (num == 3)
			cout << "ーCompare and Swap―--Thread Num: " << NUM_THREAD << endl;

		else if (num == 4)
			cout << "ーFetch and Add―--Thread Num: " << NUM_THREAD << endl;

		else
			break;

		start = clock();     //start time

		/* pthread_create */
		for (t = 0; t < NUM_THREAD; t++)
		{
			if (num == 1)
				ret = pthread_create(&threads[t], NULL, TAS, NULL);
			else if (num == 2)
				ret = pthread_create(&threads[t], NULL, TTAS, NULL);
			else if (num == 3)
				ret = pthread_create(&threads[t], NULL, CAS, NULL);
			else if (num == 4)
				ret = pthread_create(&threads[t], NULL, FAA, NULL);

			if (ret) 
			{
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
