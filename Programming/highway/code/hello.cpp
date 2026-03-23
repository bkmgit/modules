// g++ hello.cpp -o hello -I ../../../../highway/ -L ../../../../highway-install/lib64/ -lhwy_contrib  -lhwy

#include<iostream>  // std::cout
#include<sstream>   // std::isstringstream
#include "hwy/contrib/thread_pool/thread_pool.h"

namespace hwy {
namespace {

HWY_NOINLINE void ManyHellos(size_t num_threads) {
  ThreadPool pool(HWY_MIN(ThreadPool::MaxThreads(), num_threads));
  pool.Run(0, pool.NumWorkers(), [](uint64_t task, size_t thread) {
    std::cout << "Hello World from task " << task << " and thread " << thread << "." << std::endl;
    std::cout << "Goodbye World from task " << task << " and thread " << thread << "." << std::endl;
  });
}

} // namespace
} // namespace hwy

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    std::cout << "Usage:" << std::endl;
    std::cout << "./hello numthreads" << std::endl;
    std::cout << "Where numthreads is the number of threads you want to use." << std::endl;
  } else {
    std::istringstream nt(argv[2]);
    size_t num_threads;
    nt >> num_threads;
    hwy::ManyHellos(num_threads);
  }

  return 0;
}
