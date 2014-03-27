/*
 **********************************************************************************
 *  Copyright (C) 2010-2011  Massachusetts Institute of Technology
 *  Copyright (C) 2010-2011  Yuan Tang <yuantang@csail.mit.edu>
 * 		                     Charles E. Leiserson <cel@mit.edu>
 * 	 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Suggestsions:                  yuantang@csail.mit.edu
 *   Bugs:                          yuantang@csail.mit.edu
 *
 *********************************************************************************
 */

/* Test bench - 2D heat equation, Periodic version */
#include <cstdio>
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <cmath>

#include <pochoir.hpp>

using namespace std;
#define TIMES 1
#define N_RANK 2
#define TOLERANCE (1e-6)
//#define CHECK_RESULT

void check_result(int t, int j, int i, double a, double b)
{
	if (abs(a - b) < TOLERANCE) {
//		printf("a(%d, %d, %d) == b(%d, %d, %d) == %f : passed!\n", t, j, i, t, j, i, a);
	} else {
		printf("a(%d, %d, %d) = %f, b(%d, %d, %d) = %f : FAILED!\n", t, j, i, a, t, j, i, b);
	}

}

Pochoir_Boundary_2D(aperiodic_2D, arr, t, i, j)
    return 0;
Pochoir_Boundary_End

Pochoir_Boundary_2D(periodic_2D, arr, t, i, j)
    const int arr_size_1 = arr.size(1);
    const int arr_size_0 = arr.size(0);

    int new_i = (i >= arr_size_1) ? (i - arr_size_1) : (i < 0 ? i + arr_size_1 : i);
    int new_j = (j >= arr_size_0) ? (j - arr_size_0) : (j < 0 ? j + arr_size_0 : j);

    /* we use arr.get(...) instead of arr(...) to implement different boundary
     * checking strategy: In arr(...), if off-boundary access occurs, we call
     * boundary function to supply a value; in arr.get(...), if off-boundary 
     * access occurs, we will print the off-boundary access and quit!
     */
    return arr.get(t, new_i, new_j);
    // return arr.get(t, -1, -1);
Pochoir_Boundary_End

int main(int argc, char * argv[])
{
	const int BASE = 1024;
	int t;
	struct timeval start, end;
    int N1 = 500, N2 = 100, T_SIZE = 731;

    if (argc < 4) {
        printf("argc < 4, quit! \n");
        exit(1);
    }
    N1 = StrToInt(argv[1]);
    N2 = StrToInt(argv[2]);
    T_SIZE = StrToInt(argv[3]);
	
    printf("N1 = %d, N2 = %d, T_SIZE = %d\n", N1, N2, T_SIZE);
    Pochoir_Shape_2D heat_shape_2D[] = {{0, 0, 0}, {-1, 1, 0}, {-1, 0, 0}, {-1, -1, 0}, {-1, 0, -1}, {-1, 0, 1}};
    //Pochoir_Shape_2D heat_shape_2D[] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, -1, 0}, {0, 0, -1}, {0, 0, 1}};
    Pochoir<N_RANK> heat_2D(heat_shape_2D);
	Pochoir_Array<double, N_RANK> a(N1, N2) ;
    a.Register_Boundary(periodic_2D);
    heat_2D.Register_Array(a);

#ifdef CHECK_RESULT
	Pochoir_Array<double, N_RANK> b(N1, N2);
    b.Register_Shape(heat_shape_2D);
    b.Register_Boundary(periodic_2D);
#endif

    /* Now we can only access the Pochoir_Array after Register_Array,
     * or Register_Shape with the array, because we rely on the shape
     * to provide the depth of toggle array!!! 
     */
	for (int i = 0; i < N1; ++i) {
	for (int j = 0; j < N2; ++j) {
        a(0, i, j) = 1.0 * (rand() % BASE); 
        a(1, i, j) = 0; 
#ifdef CHECK_RESULT
        b(0, i, j) = a(0, i, j);
        b(1, i, j) = 0;
#endif
	} }

	cout << "a(T+1, J, I) = 0.125 * (a(T, J+1, I) - 2.0 * a(T, J, I) + a(T, J-1, I)) + 0.125 * (a(T, J, I+1) - 2.0 * a(T, J, I) + a(T, J, I-1)) + a(T, J, I)" << endl;
    Pochoir_Kernel_2D(heat_2D_fn, t, i, j)
	/*if (j <  N2  / 2 && i < N1  / 2)
	   	a(t, i, j) = 0.125 * (a(t-1, i+1, j) - 2.0 * a(t-1, i, j) + a(t-1, i-1, j)) + 0.125 * (a(t-1, i, j+1) - 2.0 * a(t-1, i, j) + a(t-1, i, j-1)) + a(t-1, i, j);
	if (j >=  N2  / 2 && i < N1  / 2)
	   	a(t, i, j) = 0.126 * (a(t-1, i+1, j) - 2.0 * a(t-1, i, j) + a(t-1, i-1, j)) + 0.125 * (a(t-1, i, j+1) - 2.0 * a(t-1, i, j) + a(t-1, i, j-1)) + a(t-1, i, j);
	if (j <  N2  / 2 && i >= N1  / 2)
	   	a(t, i, j) = 0.127 * (a(t-1, i+1, j) - 2.0 * a(t-1, i, j) + a(t-1, i-1, j)) + 0.125 * (a(t-1, i, j+1) - 2.0 * a(t-1, i, j) + a(t-1, i, j-1)) + a(t-1, i, j);
	if (j >=  N2  / 2 && i >= N1  / 2)
	   	a(t, i, j) = 0.128 * (a(t-1, i+1, j) - 2.0 * a(t-1, i, j) + a(t-1, i-1, j)) + 0.125 * (a(t-1, i, j+1) - 2.0 * a(t-1, i, j) + a(t-1, i, j-1)) + a(t-1, i, j);*/
	
	   	a(t, i, j) = 0.125 * (a(t-1, i+1, j) - 2.0 * a(t-1, i, j) + a(t-1, i-1, j)) + 0.125 * (a(t-1, i, j+1) - 2.0 * a(t-1, i, j) + a(t-1, i, j-1)) + a(t-1, i, j);
	   	//a(t + 1, i, j) = 0.125 * (a(t, i+1, j) - 2.0 * a(t, i, j) + a(t, i-1, j)) + 0.125 * (a(t, i, j+1) - 2.0 * a(t, i, j) + a(t, i, j-1)) + a(t, i, j);
    Pochoir_Kernel_End

	char name [500] ;
	sprintf(name, "heat_2D_P") ;
	heat_2D.set_problem_name(name) ;
	gettimeofday(&start, 0);
    for (int times = 0; times < TIMES; ++times) {
        heat_2D.Run(T_SIZE, heat_2D_fn);
    }
	gettimeofday(&end, 0);
	std::cout << "Pochoir ET: consumed time :" << 1.0e3 * tdiff(&end, &start)/TIMES << "ms" << std::endl;

//#if 1	
#ifdef CHECK_RESULT
	gettimeofday(&start, 0);
    for (int times = 0; times < TIMES; ++times) {
	for (int t = 0; t < T_SIZE; ++t) {
    cilk_for (int i = 0; i < N1; ++i) {
	for (int j = 0; j < N2; ++j) {
        b(t+1, i, j) = 0.125 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j); } } }
        //b(t+1, i, j) = 0.125 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j); } } }
	
	/*
	if (j <  N2  / 2 && i < N1  / 2)
	{
        b(t+1, i, j) = 0.125 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j); 
	}
	else if (j >=  N2  / 2 && i < N1  / 2)
	{
        b(t+1, i, j) = 0.126 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j); 
	}
	else if (j <  N2  / 2 && i >= N1  / 2)
	{
        b(t+1, i, j) = 0.127 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j); 
	}
	else
	{
        b(t+1, i, j) = 0.128 * (b(t, i+1, j) - 2.0 * b(t, i, j) + b(t, i-1, j)) + 0.125 * (b(t, i, j+1) - 2.0 * b(t, i, j) + b(t, i, j-1)) + b(t, i, j);   
	}*/
	}
    //}
	//}
	//}
	gettimeofday(&end, 0);
	std::cout << "Naive Loop: consumed time :" << 1.0e3 * tdiff(&end, &start)/TIMES << "ms" << std::endl;

	t = T_SIZE;
	for (int i = 0; i < N1; ++i) {
	for (int j = 0; j < N2; ++j) {
		check_result(t, i, j, a.interior(t, i, j), b.interior(t, i, j));
	} }
#endif
	return 0;
}
