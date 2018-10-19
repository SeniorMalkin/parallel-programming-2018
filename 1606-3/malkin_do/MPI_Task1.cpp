#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <mpi.h>
//#define TWOCICLE 12
#ifdef TWOCICLE
void main(int argc, char* argv[])
{
	//Read file
	FILE *file;
	char *fname = "C:\\Users\\user\\Documents\\Visual Studio 2015\\Projects\\CannyStill1\\CannyStill1\\data.txt";
	char result_string[20];
	double** endt = NULL;
	fopen_s(&file, fname, "r");

	if (file == NULL)
	{
		printf("не могу открыть файл '%s'", fname);
	}

	//Fixing size of matrix and count thread
	fgets(result_string, sizeof(result_string), file);
	int size1 = strtod(result_string, &fname);
	fname++;
	int size2 = strtod(fname, &fname);
	int size = size1*size2;
	//int size1 = 4;
	//int size2 = 4;

	//Initialization matrix
	double** arr = new double*[size1];
	for (int i = 0; i < size1; i++)
	{
		arr[i] = new double[size2];
		for (int j = 0; j < size2; j++)
		{
			arr[i][j] = 10 + rand() % 1000;

		}
	}
	double result = 0;
	clock_t time;
	time = clock();
	for (int i = 0; i < size1; i++)
	{
		for (int j = 0; j < size2; j++)
		{
			result+=arr[i][j];

		}
	}
	time = clock() - time;
	printf("%f\n", result);
	printf("%f", (double)time / CLOCKS_PER_SEC);
	system("pause");
}
#else
#define LN 10
#ifdef LN

void main(int argc, char* argv[])
{
	
	//Read file
	FILE *file;
	char *fname = "C:\\Users\\user\\Documents\\Visual Studio 2015\\Projects\\CannyStill1\\CannyStill1\\data.txt";
	char result_string[20]; 
	double** endt = NULL;
	fopen_s(&file, fname, "r");

	if (file == NULL)
	{
		printf("не могу открыть файл '%s'", fname);
	}

	//Fixing size of matrix and count thread
	fgets(result_string, sizeof(result_string), file);
	int size1 = strtod(result_string, &fname);
	fname++;
	int size2 = strtod(fname, &fname);
	int size = size1*size2;
	//int size1 = 4;
	//int size2 = 4;

	//Initialization matrix
	double** arr = new double*[size1];
	for (int i = 0; i < size1; i++)
	{
		arr[i] = new double[size2];
		for (int j = 0; j < size2; j++)
		{
			arr[i][j] = 10 + rand() % 1000;
			
		}
	}
	
	//Repack
	double* re_arr = new double[size1*size2];
	for (int i = 0; i < size1; i++)
	{
		for (int j = 0; j < size2; j++)
		{
			re_arr[i*size2 + j] = arr[i][j];

		}
	}
	

	int psize = 0;
	double res = 0;
	
	endt = arr;
	MPI_Status status;
	double result = 0;
	int ProcRank;
	double startwtime = 0.0;
	double endwtime;


	//clock_t time;
	//time = clock();
	//for (int i = 0; i < size1; i++)
	//{
	//	for (int j = 0; j < size2; j++)
	//	{
	//		result+=arr[i][j];

	//	}
	//}
	//time = clock() - time;
	//printf("%f", (double)time / CLOCKS_PER_SEC);
	//system("pause");


	//Start parallel programm
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &psize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	//Data for MPI_Scatterv
	//**********************************************************************************************************
	int* elem_proc = new int[psize];
	int* displs = new int[psize];
	int port1 = (size1*size2) / psize;
	for (int j = 0; j < psize-1 ; j++)
	{
		elem_proc[j] = port1;
		displs[j] = j*port1;
	}
	elem_proc[psize - 1] = (size1*size2) - ((psize - 1)*port1);
	displs[psize - 1] = (psize-1)*port1;
	double* recvbuf = new double[elem_proc[ProcRank]];
	for (int i = 0; i < elem_proc[ProcRank]; i++)
	{
		recvbuf[i] = 0;
	}
	MPI_Scatterv(re_arr, elem_proc, displs, MPI_DOUBLE, recvbuf, elem_proc[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//**********************************************************************************************************

	if (ProcRank == 0)
	{
		startwtime = MPI_Wtime();
	}
	
	//Summ part
	if (ProcRank == (psize - 1))
	{

		res = 0;

		for (int i = 0; i < (size1*size2) - ((psize - 1)*port1); i++)
		{
			res += recvbuf[i];
		}
		
		printf("Process %i : %f", ProcRank, res);
			
	}
		
	else
	{
		res = 0;
		for (int i = 0; i < port1; i++)
		{
			res += recvbuf[i];
		}

		printf("Process %i : %f", ProcRank, res);
	}

	
	//Reduce all part
	MPI_Reduce(&res, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (ProcRank == 0)
	{
		printf("\n %f", result);
		endwtime = MPI_Wtime();
		printf("\n Time: %f", endwtime - startwtime);
		for (int i = 0; i < size1; i++)
		{
			delete arr[i];
		}
		delete arr;
		delete elem_proc;
		delete displs;
		delete recvbuf;
	}
	MPI_Finalize();
	
}
#else
void main(int argc, char* argv[])
{
	//Read file
	FILE *file;
	char *fname = "C:\\Users\\user\\Documents\\Visual Studio 2015\\Projects\\CannyStill1\\CannyStill1\\data.txt";
	char result_string[20];
	double** endt = NULL;
	fopen_s(&file, fname, "r");

	if (file == NULL)
	{
		printf("не могу открыть файл '%s'", fname);
	}

	//Fixing size of matrix and count thread
	fgets(result_string, sizeof(result_string), file);
	int size1 = strtod(result_string, &fname);
	fname++;
	int size2 = strtod(fname, &fname);

	//Initialization matrix
	double** arr = new double*[size1];
	for (int i = 0; i < size1; i++)
	{
		arr[i] = new double[size2];
		for (int j = 0; j < size2; j++)
		{
			arr[i][j] = 10 + rand() % 1000;

		}
	}



	int psize = 0;
	double res = 0;
	double* buf = new double[size2];
	endt = arr;
	MPI_Status status;
	double result = 0;
	int ProcRank;
	double startwtime = 0.0;
	double endwtime;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &psize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);


	int portion = size1 / psize;
	if (portion == 0)
	{
	portion = 1;
	}



	if (ProcRank == 0)
	{
		res = 0;
		startwtime = MPI_Wtime();
		endt += portion;
		for (int j = 0; j < portion; j++)
		{
			for (int i = 0; i < size2; i++)
			{
				res = res + arr[j][i];
			}
		}
		printf("Process %i : %f", ProcRank, res);

		for (int i = 1; i < psize - 1; i++)
		{
			for (int j = 0; j < portion; j++)
			{
				MPI_Send(*endt, size2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
				endt++;
			}
		}
		for (int j = 0; j < size1 - portion*(psize-1); j++)
		{
		MPI_Send(*endt, size2, MPI_DOUBLE, psize-1, 0, MPI_COMM_WORLD);
		endt++;
		}
	

	}
	else {
		if (ProcRank == (psize - 1))
		{

			res = 0;
			for (int j = 0; j < size1 - portion*(psize - 1); j++)
			{
			MPI_Recv(buf, size2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
			for (int i = 0; i < size2; i++)
			{
			res = res + buf[i];
			}
			}
			printf("Process %i : %f", ProcRank, res);

		}
		else
		{

			res = 0;
			for (int j = 0; j < portion; j++)
			{
			MPI_Recv(buf, size2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
			for (int i = 0; i < size2; i++)
			{
			res = res + buf[i];
			}
			}
			printf("Process %i : %f", ProcRank, res);
		}

	}


	MPI_Reduce(&res, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (ProcRank == 0)
	{
		printf("\n %f", result);
		endwtime = MPI_Wtime();
		printf("\n Time: %f", endwtime - startwtime);
	}
	MPI_Finalize();
	for (int i = 0; i < size1; i++)
	{
		delete arr[i];
	}
	delete arr;
	delete buf;
}

#endif
#endif
