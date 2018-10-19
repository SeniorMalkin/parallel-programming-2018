#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <mpi.h>
//#define TWOCICLE 
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
		printf("íå ìîãó îòêğûòü ôàéë '%s'", fname);
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
void main(int argc, char* argv[])
{
	int ProcRank;
	int psize = 0;           // ×ÈÑËÎ İËÅÌÅÍÒÎÂ Â ÌÀÒĞÈÖÅ
	double** arr;            // ÌÀÒĞÈÖÀ
	double* re_arr;          // ÂÅÊÒÎĞ İË-ÒÎÂ ÌÀÒĞÈÖÛ
	int allSize[2];
	MPI_Status status;
	int size2;               // ×ÈÑËÎ ÑÒÎËÁÖÎÂ
	int size1;               // ×ÈÑËÎ ÑÒĞÎÊ
	int port1;               // ÏÅĞÂÎÍÀ×ÀËÜÍÀß ÏÎĞÖÈß 
	double startwtime = 0.0;
	double endwtime;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &psize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	
	if (ProcRank == 0)
	{
		
		//Read file
		FILE *file;
		char *fname = "C:\\Users\\user\\Documents\\Visual Studio 2015\\Projects\\CannyStill1\\CannyStill1\\data.txt";
		char result_string[20];
		fopen_s(&file, fname, "r");

		if (file == NULL)
		{
			printf("íå ìîãó îòêğûòü ôàéë '%s'", fname);
		}

		//Fixing size of matrix and count thread
		fgets(result_string, sizeof(result_string), file);
		size1 = strtod(result_string, &fname);
		fname++;
		size2 = strtod(fname, &fname);
		int size = size1*size2;
		//int size1 = 4;
		//int size2 = 4;

		//Initialization matrix
		arr = new double*[size1];
		for (int i = 0; i < size1; i++)
		{
			arr[i] = new double[size2];
			for (int j = 0; j < size2; j++)
			{
				arr[i][j] = 10 + rand() % 1000;
				//printf("%f  ", arr[i][j]);

			}
			//printf("\n");
		}

		//Repack
		re_arr = new double[size1*size2];
		for (int i = 0; i < size1; i++)
		{
			for (int j = 0; j < size2; j++)
			{
				re_arr[i*size2 + j] = arr[i][j];

			}
		}

		startwtime = MPI_Wtime();
		allSize[0]=size1;
		allSize[1] = size2;

	}

	MPI_Bcast(allSize, 2, MPI_INT, 0, MPI_COMM_WORLD);   

	double res = 0;    // ×ÀÑÒÈ×ÍÛÅ ÑÓÌÌÛ
	double result = 0; // ÈÒÎÃÎÂÀß ÑÓÌÌÀ
	double rem = 0;    // ÎÑÒÀÒÎÊ

	//Data for MPI_Scatterv
	//**********************************************************************************************************
	int* elem_proc = new int[psize];  // ×ÈÑËÎ İËÅÌÅÍÒÎÂ ÄËß ÎÄÍÎÃÎ ÏĞÎÖÅÑÑÀ
	int* displs = new int[psize];     // ÑÄÂÈÃ ÄËß ÊÀÆÄÎÉ ÏÎĞÖÈÈ

	if (ProcRank != 0)
	{
		size1 = allSize[0];
		size2 = allSize[1];
	}

	port1 = (size1*size2) / psize;
	rem = (size1*size2) - (port1*psize);

	// ĞÀÂÍÎÌÅĞÍÎÅ ĞÀÑÏĞÅÄÅËÅÍÈÅ ÎÑÒÀÒÊÀ,ÅÑËÈ ÎÍ ÂÅËÈÊ

	if (rem > port1)
	{
		for (int j = 0; j < rem; j++)
		{
			elem_proc[j] = (port1 + 1);
			displs[j] = j*(port1 + 1);
		}
		for (int j = rem; j < psize; j++)
		{
			elem_proc[j] = port1;
			displs[j] = rem*(port1 + 1) + port1*(j-rem);
		}
	}
	else
	{

		for (int j = 0; j < psize - 1; j++)
		{
			elem_proc[j] = port1;
			displs[j] = j*port1;
		}
		elem_proc[psize - 1] = (size1*size2) - ((psize - 1)*port1);
		displs[psize - 1] = (psize - 1)*port1;
	}

	//**********************************************************************************************************
	//SEND MESSAGE
	double* recvbuf = new double[elem_proc[ProcRank]];
	for (int i = 0; i < elem_proc[ProcRank]; i++)
	{
		recvbuf[i] = 0;
	}
	MPI_Scatterv(re_arr, elem_proc, displs, MPI_DOUBLE, recvbuf, elem_proc[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//**********************************************************************************************************
	//Summ part
	for (int i = 0; i < elem_proc[ProcRank] ; i++)
	{
	res += recvbuf[i];
	}
	printf("Process %i : %f", ProcRank, res);
	//**********************************************************************************************************
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
#endif
