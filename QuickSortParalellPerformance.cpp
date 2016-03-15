/*
*   The typical implementation of quicksort uses recursion. This
*   implementation replaces recursion with iteration. It manages its
*   own stack of unsorted sub-arrays. When the stack of unsorted
*   sub-arrays is empty, the array is sorted.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define MAX_UNFINISHED 10000
#define CHUNK_SIZE 1

struct {
    int first;
    int last;
} unfinished[MAX_UNFINISHED];  /* Stack */
int unfinished_index;
float *A;
int n;
int num_sorted;


void swap (float *x, float *y)
{
    float tmp;
    tmp = *x;
    *x = *y;
    *y = tmp;
}


/* Function 'partition' actually does the sorting by dividing an
Unsorted sub-array into two parts: those less than or equal to the
splitter, and those greater than the splitter. The splitter is the
last element in the unsorted sub-array. The splitter ends up in its
final, sorted location. The function returns the final location of
the splitter (its index). */
int partition (int first, int last)
{
    int i, j;
    float x;
    x = A[last];
    i = first - 1;
    for (j = first; j < last; j++)
    if (A[j] <= x) {
        i++;
        swap (&A[i], &A[j]);
    }
    swap (&A[i+1], &A[last]);
    return (i+1);
}

/* Function `seq_quicksort’ implements the traditional, recursive
quicksort algorithm. It is called when we want a thread to be
be responsible for sorting an entire sub-array. */
void seq_quicksort (int first, int last)
{
    int q;    /* Split point in array */
    if (first < last) {
        q = partition (first, last);
        seq_quicksort (first, q-1);
        seq_quicksort (q+1, last);
} }


/* Function 'quicksort' repeatedly retrieves the indices of unsorted
sub-arrays from the stack and calls 'partition' to divide these
sub-arrays into two pieces. It keeps one of the pieces and puts the
other piece on the stack of unsorted sub-arrays. Eventually it ends
up with a piece that doesn't need to be sorted. At this point it
gets the indices of another unsorted sub-array from the stack. The
function continues until the stack is empty.
*/
void quicksort (void)
{
    int first;
    int id;
    int last;
    int my_count;
    int my_index;
    int q;    
    id = omp_get_thread_num();
    printf ("Thread %d enters quicksortn", id);
    my_count = 0;
    while (num_sorted < n) {
        #pragma omp critical
        {
            if (unfinished_index >= 0) {
                my_index = unfinished_index;
                unfinished_index--;
                first = unfinished[my_index].first;
                last = unfinished[my_index].last;
                } else {
                first = 0;
            last = -1; }
        }

        while (first <= last) {

            if (first == last) {
                #pragma omp critical
                num_sorted++;
                my_count++;
                last = first - 1;
                } else if ((last - first) < CHUNK_SIZE) {
                seq_quicksort (first, last);
                #pragma omp critical
                num_sorted += (last - first + 1);
                my_count += (last - first + 1);
                last = first - 1;
                } else {

                q = partition (first, last);
                #pragma omp critical
                num_sorted++;
                my_count++;

                if ((unfinished_index+1) >= MAX_UNFINISHED) {
                    printf ("Stack overflown");
                exit (-1); }
                #pragma omp critical
                { }
                id, my_count);
            }

            void print_float_array (float *A, int n)
            {
                int i;
                printf ("Contents of array:n");
                for (i = 0; i < n; i++)
                printf ("%6.4fn", A[i]);
            }

            int verify_sorted (float *A, int n)
            {
                int i;
                for (i = 0; i < n-1; i++)
                if (A[i] > A[i+1]) return 0;
            return 1; }
        }
        int main (int argc, char *argv[])
        {
            int    i;
            int    seed;
            unsigned short xi[3];
            int    t;

            if (argc != 4) {
                printf ("Command-line syntax: %s <n> <threads> <seed>n",
                argv[0]);
                exit (-1);
            }
            seed = atoi (argv[3]);
            xi[0] = xi[1] = xi[2] = seed;
            t = atoi(argv[2]);
            omp_set_num_threads (t);
            n = atoi (argv[1]);
            A = (float *) malloc (n * sizeof(float));
            for (i = 0; i < n; i++)
            A[i] = erand48(xi);
            
            /*
            print_float_array (A, n);
            */
            
            unfinished[0].first = 0;
            unfinished[0].last = n-1;
            unfinished_index = 0;
            num_sorted = 0;
            #pragma omp parallel
            quicksort ();
            
            /*
            print_float_array (A, n);
            */
            
            if (verify_sorted (A, n)) printf ("Elements are sortedn");
            else printf ("ERROR: Elements are NOT sortedn");
            return 0;
        }
