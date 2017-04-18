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
		while (lock.exchange(true, memory_order_acquire))
		{
		}
		sum++;
		lock.store(false, memory_order_release);
	}
	return (void *)0;
}

//Test and Test and Set
void *TTAS(void *arg)
{
	for (long int i = 0; i < Round; i++)
	{
		/*while (lock_stream)
		{
		}
		while (lock_stream.exchange(true))
		{
		}
		sum++;
		lock_stream = false;
		*/
		while (1)
		{
			if (!lock_stream.load(memory_order_relaxed))
			{
				if (!lock_stream.exchange(true, memory_order_acquire))
					break;
			}
		}
		sum++;
		lock_stream.store(false, memory_order_release);
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
		turn++;
	}
	/*for (long int i = 0; i < Round; i++)
		atomic_fetch_add(&sum, 1);*/
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
	pthread_t threads[12];

	int ret, t;
	double runtime;
	double TAS_time, TTAS_time, CAS_time, FAA_time;
	long int i;
	//Round = 100000000 / NUM_THREAD;
	printf("         TAS        TTAS        CAS        FAA\n");

	for (i = 1; i < 9; i++)
	{
		Round = 100000000 / i;  //Set round

		ticketnumber = 0;       //Fetch and Add lock
		turn = 0;

		SetColor(7, 0);
		cout << "Thread" << i << "  ";

		//Initial sum
		sum = 0;
		/************Test and Set************/
		start = clock();        //start time

	    /* pthread_create */
		for (t = 0; t < i; t++)
		{
			ret = pthread_create(&threads[t], NULL, TAS, NULL);

			if (ret)
			{
				cout << " create thread error " << endl;
				return -1;
			}
		}
		/* pthread_join */
		for (t = 0; t < i; t++)
			ret = pthread_join(threads[t], NULL);

		finish = clock();     //finish time

		runtime = double(finish - start) / CLOCKS_PER_SEC;
		SetColor(14, 0);
		printf("%.3f s    ", runtime);

		//Initial sum
		sum = 0;
		/************Test and Test and Set************/
		start = clock();     //start time

		/* pthread_create */
		for (t = 0; t < i; t++)
		{
			ret = pthread_create(&threads[t], NULL, TTAS, NULL);

			if (ret)
			{
				cout << " create thread error " << endl;
				return -1;
			}
		}

		/* pthread_join */
		for (t = 0; t < i; t++)
			ret = pthread_join(threads[t], NULL);

		finish = clock();     //finish time

		runtime = double(finish - start) / CLOCKS_PER_SEC;

		SetColor(14, 0);
		printf("%.3f s    ", runtime);
		
		//Initial sum
		sum = 0;
		/************Compare and Swap************/
		start = clock();     //start time

    	/* pthread_create */
		for (t = 0; t < i; t++)
		{
			ret = pthread_create(&threads[t], NULL, CAS, NULL);

			if (ret)
			{
				cout << " create thread error " << endl;
				return -1;
			}
		}
		/* pthread_join */
		for (t = 0; t < i; t++)
			ret = pthread_join(threads[t], NULL);

		finish = clock();     //finish time

		runtime = double(finish - start) / CLOCKS_PER_SEC;

		SetColor(14, 0);
		printf("%.3f s    ", runtime);
		
		//Initial sum
		sum = 0;
		/************Fetch and Add************/
		start = clock();     //start time

		/* pthread_create */
		for (t = 0; t < i; t++)
		{
			ret = pthread_create(&threads[t], NULL, FAA, NULL);

			if (ret)
			{
				cout << " create thread error " << endl;
				return -1;
			}
		}
		/* pthread_join */
		for (t = 0; t < i; t++)
			ret = pthread_join(threads[t], NULL);

		finish = clock();     //finish time

		runtime = double(finish - start) / CLOCKS_PER_SEC;

		SetColor(14, 0);
		printf("%.3f s\n", runtime);
	}
	getchar();
	getchar();
	return 0;
}

//#define HAVE_STRUCT_TIMESPEC
//#include <pthread.h>
//#include <atomic>
//#include <time.h>
//#include <windows.h>
//#include <iostream>
//#define NUM_THREAD 8
////#define CACHELINE_SIZE 32
//
//using namespace std;
//#pragma comment(lib,"pthreadVC2.lib") 
//
//atomic <int> ticketnumber = ATOMIC_VAR_INIT(0);
//atomic <int> turn;
//
//atomic <long int> sum; atomic <long int> flag;
////atomic_flag lock = ATOMIC_FLAG_INIT;
//atomic <bool> lock(false);
//atomic <bool> lock_stream = ATOMIC_FLAG_INIT;
////alignas(CACHELINE_SIZE) 
//atomic_bool locked_tas = { false };
////alignas(CACHELINE_SIZE) 
//atomic_bool locked_ttas = { false };
//clock_t start, finish;
//long int Round;
//bool lock1 = false;
//
////Test and Set
//void *TAS(void *arg)
//{
//	for (long int i = 0; i < Round; i++)
//	{
//#if 0
//		while (lock.exchange(true))
//		{
//		}
//		sum++;
//		lock = false;
//#endif
//		while (locked_tas.exchange(true, memory_order_acquire) == true)
//		{
//		}
//		sum++;
//		locked_tas.store(false, memory_order_release);
//	}
//	return (void *)0;
//}
//
////Test and Test and Set
//void *TTAS(void *arg)
//{
//	for (long int i = 0; i < Round; i++)
//	{
//#if  0
//		while (lock_stream)
//		{
//		}
//		while (lock_stream.exchange(true))
//		{
//		}
//		sum++;
//		lock_stream = false;
//	}
//#else
//		do {
//			while (locked_ttas.load(memory_order_relaxed) == true)
//			{
//			}
//		} while (locked_ttas.exchange(true, memory_order_acquire) == true);
//		sum++;
//		locked_ttas.store(false, memory_order_release);
//}
//#endif
//	return (void *)0;
//}
//
////Fetch and Add
//void *FAA(void *arg)
//{
//	long int myturn;
//	for (long int i = 0; i < Round; i++)
//	{
//		myturn = atomic_fetch_add(&ticketnumber, 1);
//		while (myturn != turn)
//		{
//		}
//		sum++;
//		//atomic_fetch_add(&turn, 1);
//		turn++;
//	}
//	return (void *)0;
//}
//
////Compare and Swap
//void *CAS(void *arg)
//{
//	long cur_sum;
//	for (int i = 0; i< Round; i++)
//	{
//		cur_sum = sum;
//		bool flag = sum.compare_exchange_weak(cur_sum, cur_sum + 1);
//		if (!flag)
//			i = i - 1;
//	}
//	return (void *)0;
//}
//
////Change character's color
//void SetColor(unsigned short ForeColor, unsigned short BackGroundColor)
//{
//	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
//	SetConsoleTextAttribute(hCon, (ForeColor % 16) | (BackGroundColor % 16 * 16));
//}
//
//int main()
//{
//	int num;
//	//pthread_t threads[NUM_THREAD];
//	pthread_t threads[17];
//
//	int ret, t;
//	double runtime;
//	double TAS_time, TTAS_time, CAS_time, FAA_time;
//	long int i;
//	//	Round = 100000000 / NUM_THREAD;
//	printf("         TAS        TTAS        CAS        FAA\n");
//
//	for (i = 1; i < 8; i++)
//	{
//		Round = 100000000 / i;  //Set round
//
//		ticketnumber = 0;       //Fetch and Add lock
//		turn = 0;
//
//		SetColor(7, 0);
//		cout << "Thread" << i << "  ";
//
//		/************Test and Set************/
//		start = clock();        //start time
//
//								/* pthread_create */
//		for (t = 0; t < i; t++)
//		{
//			ret = pthread_create(&threads[t], NULL, TAS, NULL);
//
//			if (ret)
//			{
//				cout << " create thread error " << endl;
//				return -1;
//			}
//		}
//		/* pthread_join */
//		for (t = 0; t < i; t++)
//			ret = pthread_join(threads[t], NULL);
//
//		finish = clock();     //finish time
//
//		runtime = double(finish - start) / CLOCKS_PER_SEC;
//		SetColor(14, 0);
//		printf("%.3f s    ", runtime);
//		//Initial sum
//		sum = 0;
//		/************Test and Test and Set************/
//		start = clock();     //start time
//
//							 /* pthread_create */
//		for (t = 0; t < i; t++)
//		{
//			ret = pthread_create(&threads[t], NULL, TTAS, NULL);
//
//			if (ret)
//			{
//				cout << " create thread error " << endl;
//				return -1;
//			}
//		}
//
//		/* pthread_join */
//		for (t = 0; t < i; t++)
//			ret = pthread_join(threads[t], NULL);
//
//		finish = clock();     //finish time
//
//		runtime = double(finish - start) / CLOCKS_PER_SEC;
//
//		SetColor(14, 0);
//		printf("%.3f s    ", runtime);
//		//Initial sum
//		sum = 0;
//		/************Compare and Swap************/
//		start = clock();     //start time
//
//							 /* pthread_create */
//		for (t = 0; t < i; t++)
//		{
//			ret = pthread_create(&threads[t], NULL, CAS, NULL);
//
//			if (ret)
//			{
//				cout << " create thread error " << endl;
//				return -1;
//			}
//		}
//		/* pthread_join */
//		for (t = 0; t < i; t++)
//			ret = pthread_join(threads[t], NULL);
//
//		finish = clock();     //finish time
//
//		runtime = double(finish - start) / CLOCKS_PER_SEC;
//
//		SetColor(14, 0);
//		//cout << runtime << " s    ";
//		printf("%.3f s    ", runtime);
//		//Initial sum
//		sum = 0;
//		/************Fetch and Add************/
//		start = clock();     //start time
//
//		/* pthread_create */
//		for (t = 0; t < i; t++)
//		{
//			ret = pthread_create(&threads[t], NULL, FAA, NULL);
//
//			if (ret)
//			{
//				cout << " create thread error " << endl;
//				return -1;
//			}
//		}
//		/* pthread_join */
//		for (t = 0; t < i; t++)
//			ret = pthread_join(threads[t], NULL);
//
//		finish = clock();     //finish time
//
//		runtime = double(finish - start) / CLOCKS_PER_SEC;
//
//		SetColor(14, 0);
//		//cout << runtime << " s    "<< endl;
//		printf("%.3f s\n", runtime);
//	}
//	getchar();
//	getchar();
//	return 0;
//}
