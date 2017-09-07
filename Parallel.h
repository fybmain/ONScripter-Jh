/* -*- C++ -*-
 *
 *  Parallel.h
 *
 *  Copyright (C) 2014-2016 jh10001 <jh10001@live.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once

#if defined(USE_PARALLEL) && defined(USE_OMP_PARALLEL)
#undef USE_PARALLEL
#endif

#ifdef USE_PARALLEL
#include "SDL_cpuinfo.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "SDL_atomic.h"
#endif

#ifdef USE_OMP_PARALLEL
#include <omp.h>
#endif

#if defined(USE_PARALLEL) || defined(USE_OMP_PARALLEL)
#include <assert.h>

namespace parallel {
#ifdef USE_OMP_PARALLEL
  static const int thread_num = omp_get_num_procs();
#elif defined(USE_PARALLEL)
  static const int thread_num = SDL_GetCPUCount();

  class ThreadPool {
    struct Thread {
      enum Status {
        IDLE, RUNNING, EXIT
      };
      SDL_Thread *thread;
      struct ThreadData {
        void(*body)(void *data);
        void *data;
        SDL_atomic_t status;
        SDL_sem *sem;
      } threadData;
    };
    Thread *threads = new Thread[thread_num];
    int threadNum = 0, threadCreated = 0;
    inline void createThread() {
      assert(threadCreated < thread_num);
      Thread *thread = &threads[threadCreated];
      thread->threadData.sem = SDL_CreateSemaphore(0);
      thread->thread = SDL_CreateThread([](void *ptr) {
        Thread::ThreadData *td = (Thread::ThreadData*) ptr;
        for (;;) {
          td->body(td->data);
          int status = Thread::Status::IDLE;
          SDL_AtomicSet(&td->status, status);
          while (status != Thread::Status::RUNNING) {
            if (status == Thread::Status::EXIT) goto exit_thread;
            SDL_SemWait(td->sem);
            status = SDL_AtomicGet(&td->status);
          }
        }
      exit_thread:
        SDL_DestroySemaphore(td->sem);
        return 0;}, "Parallel", (void*)&thread->threadData);
      SDL_DetachThread(thread->thread);
      ++threadCreated;
    }
  public:
    inline ~ThreadPool() {
      sync();
      for (int i = 0; i < threadCreated; ++i) {
        SDL_AtomicSet(&threads[i].threadData.status, Thread::Status::EXIT);
        SDL_SemPost(threads[i].threadData.sem);
      }
    }
    inline Thread* newThread(void(*func)(void *body), void* threadData) {
      assert(threadNum < thread_num);
      Thread *thread = &threads[threadNum];
      Thread::ThreadData &td = thread->threadData;
      td.body = func;
      td.data = threadData;
      SDL_AtomicSet(&td.status, Thread::Status::RUNNING);
      if (threadNum + 1 > threadCreated) createThread();
      else SDL_SemPost(td.sem);
      ++threadNum;
      return thread;
    }
    inline void sync() {
      for (int i = threadNum - 1; i >= 0; --i) {
        while (SDL_AtomicGet(&threads[i].threadData.status) != Thread::Status::IDLE) SDL_Delay(0);
        --threadNum;
      }
    }
  };

  extern ThreadPool threadPool;
#endif
  static int thread_clamp(int threadnum) {
    if (threadnum > thread_num) threadnum = thread_num;
    if (threadnum < 1) threadnum = 1;
    return threadnum;
  }

  template<typename Body>
  void For(const int first, const int last, const int step, const Body &body, const int scale = -1) {
    assert(step > 0);
    if (last > first) {
      static const int MINSCALE = 65536 * 4;
#ifdef USE_OMP_PARALLEL
      scale > 0 ? omp_set_num_threads(thread_clamp(scale / MINSCALE)) : omp_set_num_threads(thread_num);
#pragma omp parallel for
      for (int i = first; i < last; i += step) body(i);
#elif defined USE_PARALLEL
      int range = last - first;
      struct ThreadData {
        int lr[2];
        const Body *body;
      };
      int nthread = scale > 0 ? thread_clamp(scale / MINSCALE) : thread_num;
      if (range < nthread) nthread = range;
      typedef void(*FunctionBody)(void*);
      FunctionBody* funcs = new FunctionBody[nthread - 1];
      ThreadData *td = new ThreadData[nthread - 1];
      int ssize = range / nthread;
      int lend = last;
      int i = nthread;
      while (i > 1) {
        int lstart = lend - ssize;
        td[i - 2] = { { lstart, lend }, &body };
        funcs[i - 2] = [](void* ptr) {
          ThreadData &td = *((ThreadData*)ptr);
          for (int i = td.lr[0]; i < td.lr[1]; ++i) (*td.body)(i);};
        threadPool.newThread(funcs[i - 2], (void*)&td[i - 2]);
        lend = lstart;
        --i;
      }
      for (int i = first; i < lend; ++i) body(i);
      threadPool.sync();
      delete[] td;
      delete[] funcs;
#endif
    }
  }
}
#endif
