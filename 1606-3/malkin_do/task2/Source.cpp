#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <mpi.h>
#include <math.h>


int powTwo(int proc) {
	int res = 0;
	while (proc != 0) {
		proc = proc >> 1;
		res++;
	}
	return res;
}
int twoin(int pow) {
	int res = 1;
	while (pow != 0) {
		res << 1;
		pow--;
	}
	return res;
}
void My_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	int currRank;
	int psize;
	int tut;

	MPI_Status status;
	
	MPI_Comm_size(comm, &psize);
	MPI_Comm_rank(comm, &currRank);

	int ther = powTwo(psize);
	
	if (currRank != root) {
		MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE,MPI_ANY_TAG, comm, &status);
	}
	else {
		if (root != 0) {
			MPI_Send(buffer, count, datatype, 0, 0, comm);
		}
	}

	/*
	for (int i = 0; i < ther - 1; i++) {
		tut = 1 << i;
		for (int j = 0; j < tut; j++) {
				if ((currRank == j) && ((j+tut)!= root))
					MPI_Send(buffer, count, datatype, j + tut, 0, comm);
		}

	}
	*/
	
		
	for (int i = powTwo(currRank); (currRank | (1 << i)) < psize; i++) {
		if ((currRank | (1 << i)) != root)
		MPI_Send(buffer, count, datatype, currRank | (1 << i), 0, comm);
	}
	
	
	/*
	tut = 1 << (ther - 1);
	for (int i = 0; i < psize - tut; i++) {
		if (i + tut != root) {
			if (currRank == i)
				MPI_Send(buffer, count, datatype, i + tut , 0, comm);
		}
	}
	*/
}


void main(int argc, char* argv[])
{
	int ProcRank;
	int psize = 0;
	double* test = nullptr;
	double startwtime = 0.0;
	double endwtime , startwtime1,endwtime1;
	int tut = powTwo(5);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &psize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	test = new double[4];
	if (ProcRank == 2){
		test[0] = 1.12;
		test[1] = 1.23;
		test[2] = 2.44;
		test[3] = 3.12;
	}
	if(ProcRank == 0)
	startwtime = MPI_Wtime();
	My_Bcast(test,4, MPI_DOUBLE,2, MPI_COMM_WORLD);
	if (ProcRank == 0)
	endwtime = MPI_Wtime();

	if (ProcRank == 0)
	startwtime1 = MPI_Wtime();
	MPI_Bcast(test, 4, MPI_DOUBLE,2, MPI_COMM_WORLD);
	if (ProcRank == 0)
	endwtime1 = MPI_Wtime();

	for (int i = 0; i < psize; i++) {
		if (ProcRank == i) {
			printf("Process %i : %f\n", ProcRank, test[2]);
		}
	}
	
	if (ProcRank == 0) {
		printf("Time my function: %f\n", endwtime - startwtime);
		printf("Time standart function: %f", endwtime1 - startwtime1);
	}
	MPI_Finalize();
	
	
}