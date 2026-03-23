/* 
 *  Highway threads implementation of dot product calculation. This program is used as the driving example in the 
 *  Demos in the module Heterogeneous Programming with Highway
 *
 *  @author Apan Qasem 
 */
// g++ dot_product.cpp -o dot_product -I ../../../../highway/ -L ../../../../highway-install/lib64/ -lhwy_contrib  -lhwy

#include<iostream>  // std::cout
#include<sstream>   // std::istringstream
#include<stdlib.h>
#include<sys/time.h>

#include "hwy/contrib/thread_pool/thread_pool.h"

#define REPS 1000

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

HWY_NOINLINE void add(size_t num_threads, size_t vector_length, size_t & workers, float a[], float b[], float c[]) {
  ThreadPool pool(HWY_MIN(ThreadPool::MaxThreads(), num_threads));
  workers = pool.NumWorkers() - 1;
  pool.Run(0, workers, [=](uint64_t task, size_t thread) {
    float local_sum = 0.0;
    size_t low = task * (vector_length  / workers);
    size_t high;
    if (task != workers - 1) {
      high = (task + 1) * (vector_length / workers);
    }else{
      high = vector_length;
    }
    for (size_t i = low; i < high ; i++){
      local_sum += a[i] * b[i];
    }
    c[task] = local_sum;
  });
}

} // namespace
} // namespace hwy

int main(int argc, char *argv[]) {

  if (argc <= 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "./dot_product vector_length num_threads" << std::endl;
    std::cout << "Where vector_length is the length of the vector for the" << std::endl;
    std::cout << "dot product and num_threads is the number of threads to use." << std::endl;
  }else{
    std::istringstream vl(argv[1]);
    std::istringstream nt(argv[2]);
    size_t vector_length;
    size_t num_threads;
    vl >> vector_length;
    nt >> num_threads;
    float *a = NULL;
    float *b = NULL;
    float *c = NULL;
    a = new float[vector_length];
    b = new float[vector_length];
    c = new float[num_threads];

    for (size_t i = 0; i < vector_length; i++) {
      a[i] = i; 
      b[i] = i + 3; 
    }

    float sum;
    size_t num_workers;
    t0 = mysecond();
    for (size_t j = 0; j < REPS; j++) {
      sum = 0;
      hwy::add(num_threads,vector_length,num_workers,a,b,c);
      for (size_t i = 0; i < num_workers; i++) {
	sum += c[i];
      }
    }
    t0 = ((mysecond() - t0) * 1.e3) / REPS;

    delete [] a;
    delete [] b;
    delete [] c;

    std::cout << "Work threads used = " << num_workers << std::endl;
    std::cout << "Result = " << sum << std::endl;
    std::cout << "Parallel loop time " << t0 << "ms" << std::endl;
  }
  return 0;

}
