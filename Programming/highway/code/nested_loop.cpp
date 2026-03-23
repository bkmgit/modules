/* 
 *  This example illustrates nested loop parallelization with OpenMP
 *
 *  @author Apan Qasem 
 */
// g++ nested_loop.cpp -o nested_loop -I ../../../../highway/ -L ../../../../highway-install/lib64/ -lhwy_contrib  -lhwy

#include<iostream>  // std::cout
#include<sstream>   // std::istringstream
#include<stdlib.h>
#include<stdio.h>

#include "hwy/contrib/thread_pool/thread_pool.h"

#define M 4

namespace hwy {
namespace {

HWY_NOINLINE void nested_loop(size_t num_threads, size_t & workers) {
  ThreadPool pool(HWY_MIN(ThreadPool::MaxThreads(), num_threads));
  workers = pool.NumWorkers() - 1;
  pool.Run(0, workers, [=](uint64_t task, size_t thread) {
    for (size_t i = 0; i < M ; i++){
      std::cout<< "I am thread " << thread << " in iteration (" << i << "," << task << ")" << std::endl;
    }
  });
}

} // namespace
} // namespace hwy

int main(int argc, char *argv[]) {
  
  if (argc <= 1) {
    std::cout << "Usage:" << std::endl;
    std::cout << "./nested_loop num_threads" << std::endl;
    std::cout << "Where num_threads is the number of threads to use." << std::endl;
  }else{
    std::istringstream nt(argv[1]);
    size_t num_threads;
    nt >> num_threads;

    size_t num_workers;
    hwy::nested_loop(num_threads,num_workers);
    std::cout << "Work threads used = " << num_workers << std::endl;
  }
  return 0;
}
