/* 
 *  Highway implementatation of scaling a 2D array. This code is used to illustrate benefits of 
 *  multi-threaded parallelism and limits on performance scalability (i.e., Amdahl's Law)
 *
 *  @author Apan Qasem 
 */
// g++ scale.cpp -o scale -I ../../../../highway/ -L ../../../../highway-install/lib64/ -lhwy_contrib  -lhwy

#include<iostream>  // std::cout
#include<sstream>   // std::istringstream
#include<stdlib.h>
#include<sys/time.h>

#include "hwy/contrib/thread_pool/thread_pool.h"

#define REPS 100

double t0;
double mysecond() {
  struct timeval tp;
  struct timezone tzp;
  int i;

  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

namespace hwy {
namespace {

HWY_NOINLINE void scale(size_t num_threads, size_t root_array_size, size_t & workers, float *a, float *b) {
  ThreadPool pool(HWY_MIN(ThreadPool::MaxThreads(), num_threads));
  workers = pool.NumWorkers() - 1;
  pool.Run(0, workers, [=](uint64_t task, size_t thread) {
    size_t low = task * (root_array_size  / workers);
    size_t high;
    if (task != workers - 1) {
      high = (task + 1) * (root_array_size / workers);
    }else{
      high = root_array_size;
    }
    for (size_t j = low; j < high; j++) {
      for (size_t i = 0; i < root_array_size; i++) {
        a[i + (j * root_array_size)] = b[i + (j * root_array_size)] * 17;
      }
    }
  });
}

} // namespace
} // namespace hwy

int main(int argc, char *argv[]) {

  if (argc <= 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "./scale root_array_size num_threads" << std::endl;
    std::cout << "Where root_array_size is the width of the square array to be" << std::endl;
    std::cout << "scaled and num_threads is the number of threads to use." << std::endl;
  }else{
    std::istringstream ras(argv[1]);
    std::istringstream nt(argv[2]);
    size_t root_array_size;
    size_t num_threads;
    ras >> root_array_size;
    nt >> num_threads;
    float *a = NULL;
    float *b = NULL;
    a = new float[root_array_size * root_array_size];
    b = new float[root_array_size * root_array_size];

    for (size_t j = 0; j < root_array_size; j++) {
      for (size_t i = 0; i < root_array_size; i++) {
        b[i + (j * root_array_size)] = i + j;
      }
    }

    size_t num_workers;
    t0 = mysecond();
    for (size_t k = 0; k < REPS; k++) {
      hwy::scale(num_threads,root_array_size,num_workers,a,b);
    }

    t0 = ((mysecond() - t0) * 1.e3) / REPS;

    delete [] a;
    delete [] b;
    std::cout << "Work threads used = " << num_workers << std::endl;
    std::cout << "Parallel loop time " << t0 << "ms" << std::endl;
  }
  return 0;

}
