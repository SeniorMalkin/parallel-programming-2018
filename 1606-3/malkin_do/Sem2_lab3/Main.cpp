
#include "tbb/tbb.h"
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <omp.h>

template<class Iterator>
class Quick_sorter :public tbb::task
{
    Iterator itbegin_, itend_;
    unsigned int grain_size_;
public:

    static const unsigned int defaultGrainSize = 2000;

    task* execute()
    {
        tbb::task_list list;
        Iterator itbegin = itbegin_,
            itend = itend_;
        const unsigned int sort_range_size =
            (unsigned int)std::distance(itbegin, itend);
        if (sort_range_size<2)
            return NULL;
        Iterator mid = itbegin_;
        std::advance(mid, sort_range_size / 2);
       
        if (sort_range_size < get_grain_size()) {
            q_sort<Iterator>(itbegin, itend, sort_range_size);
        }
        else
        {
            Quick_sorter<Iterator>& c1 = *new(tbb::task::allocate_child()) Quick_sorter<Iterator>(itbegin, mid);
            Quick_sorter<Iterator>& c2 = *new(tbb::task::allocate_child()) Quick_sorter<Iterator>(mid + 1, itend);
            tbb::task::set_ref_count(3);
            tbb::task::spawn_and_wait_for_all(list);
        }
        return NULL;
    }

    unsigned int get_grain_size()const {
        return grain_size_;
    }
    void set_grain_size(unsigned int new_grain_size) {
        grain_size_ = new_grain_size;
    }

    Quick_sorter(Iterator itbeg, Iterator ite, unsigned int grain_size = defaultGrainSize) :
        itbegin_(itbeg), itend_(ite), grain_size_(grain_size)
    {}
};

template<class Iterator> void
quicksort_tbb(Iterator itbegin,
    Iterator itend,
    unsigned int grain_size = Quick_sorter<Iterator>::defaultGrainSize
) {
    Quick_sorter<Iterator> &sorter
        = *new (tbb::task::allocate_root()) Quick_sorter<Iterator>(itbegin, itend,grain_size);
    tbb::task::spawn_root_and_wait(sorter);
}

template<class Iterator>
void q_sort(Iterator left, Iterator right, const unsigned int sort_range_size) {

    /*
        int piv = (left + right) / 2;
        int r = right;
        int l = left;*/
    Iterator mid = left;
    std::advance(mid, sort_range_size / 2);
    while (*left <= *right) {
        while (*left < *mid)
            left++;
        while (*right > *mid)
            right--;
        if (*left <= *right)
        {
            std::iter_swap(left++, right--);
            /*int buf = arr[l];
            arr[l++] = arr[r];
            arr[r--] = buf;*/
        }
    }
}



int main() {
    srand(time(0));

    int size = 10000;
    //int* arr = new int[size];
    tbb::concurrent_vector<int> vec(10000);
    for (int i = 0; i < size; i++) {
        vec.push_back(rand()%100 - 50);
        //arr[i] = i;
        //printf("%d ", arr[i]);
    }
    printf("\n");

    //int start = clock();
    quicksort_tbb<tbb::internal::vector_iterator<tbb::concurrent_vector<int>,int>>(vec.begin(), vec.end()-1);
    //int finish = clock();
    //printf("%d ", finish - start);
    system("pause");
    return 0;
    /*
    for (int i = 0; i < size; i++)
    printf("%d ", arr[i]);
    */
}