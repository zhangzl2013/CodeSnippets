#! /usr/bin/env python

from __future__ import print_function
from threading import Thread
import subprocess
import os
import time
import random
import sys


def pstree(main_pid):
     for i in range(0, 100):
         try:
             FNULL = open(os.devnull, 'w')
             retcode = subprocess.call(["pstree", "-p", main_pid] , 
stdout=FNULL)
             FNULL.close()
             if retcode < 0:
                 print("pstree was terminated by signal", -retcode, file=sys.stderr)
         except OSError as e:
             print("Ececution failed:", e, file=sys.stderr)


def random_sleep():
     #print('sub-process: {} # {}'.format(os.getpid(), i))
     sleep_time = random.random() * 1
     time.sleep(sleep_time)
     #print("result : {}".format(sleep_time))


if __name__=='__main__':
     print('Main Process id: {}'.format(os.getpid()))
     main_pid = str(os.getpid())
     start = time.time()

     task_pstree = Thread(target=pstree, args=(main_pid, ))
     task_pstree.start()
     for j in range(0, 100):
         tasks = []
         for i in range(0, 100):
             tasks.append(Thread(target=random_sleep))
         print('waiting for sub-thread {}'.format(j))
         for i in range(1, 100):
             tasks[i].start()
         for i in range(1, 100):
             tasks[i].join()

     task_pstree.join()
     end = time.time()
     print("totally use {} seconds".format((end - start)))

