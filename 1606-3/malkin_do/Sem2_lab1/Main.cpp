
#include <stdio.h>
#include <cstdlib>
#include <ctime>
void q_sort(int left, int right, int* arr){
	if (left < right)
	{
		int piv = (left + right) / 2;
		int r = right;
		int l = left;
		int mid = arr[piv];
		while (l <= r) {
			while (arr[l] < mid)
				l++;
			while (arr[r] > mid)
				r--;
			if (l <= r)
			{
				int buf = arr[l];
				arr[l++] = arr[r];
				arr[r--] = buf;
			}
		}
		q_sort(left, r, arr);
		q_sort(l, right, arr);
	}
}



void main() {
	srand(time(0));

	int size = 100000;
	int* arr = new int[size];

	for (int i = 0; i < size; i++) {
		//arr[i] = rand()%100 - 50;
		arr[i] = i;
		//printf("%d ", arr[i]);
	}
	printf("\n");
	
	int start = clock();
	q_sort(0, size-1, arr);
	int finish = clock();
	/*
	for (int i = 0; i < size; i++)
	printf("%d ", arr[i]);
	*/
	printf("%d ", finish - start);
	getchar();

}