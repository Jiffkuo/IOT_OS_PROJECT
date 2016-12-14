/**
 * \file
 *         Refined pChase benchmark for memory performance
 * \author
 *         Tzu-Chi Kuo
 * \target
 *         make;
 *         cd bin/z1; ln -s pChase.elf pChase.z1
 * \execution
 *         cd /home/user/contiki-3.0/tools/cooja
 *         ant run
 *         File -> New Simulation
 *         Motes -> Add motes -> Create new mote type -> Z1 mote
 *         Browse -> pChase.z1 -> Create -> Start
 * \Reference
 *         https://github.com/maleadt/pChase
 */

#include "xtimer.h"	/*RITO timer lib */
#include <stdio.h>      /* For printf() */
#include <stdlib.h>     /* For sizeof() */

/* global definitation*/
// bytes
/*
#define BYTES_PER_LINE   32
#define BYTES_PER_PAGE   4096
*/
#define CHAIN_SIZE       8192 //8192
static int BYTES_PER_LINE[5] = {8, 16, 32, 64, 128};
static int BYTES_PER_PAGE[5] = {512, 1024, 2048, 4096, 8192};
#define KB               1024
// number
#define CHAIN_PER_THREAD  1
#define NUM_OF_THREADS    1
#define NUM_OF_EXPRIMENTS 5
#define NUM_OF_ITERATION  100
// time unit
#define NS 1E9
#define US 1E6
#define MS 1E3
// bandwidth
#define MB 1E-6

/* global variable */
// chain pointer
typedef struct Chain {
  struct Chain* _next;
} Chain;
typedef Chain *pChain;

/*---------------------------------------------------------------------------*/
int main(void)
{
  // for time calculation
  unsigned long startTime = 0;
  unsigned long endTime = 0;
  // for loop count
  unsigned int i = 0;
  unsigned int j = 0; // target on BYTES_PER_LINES
  unsigned int k = 0; // target on BYTES_PER_PAGE
  // for latency and performance calculation
  unsigned int numOfOperation = 0;
  // key parameters
  unsigned int pagesPerChain = 0; // CHAIN_SIZE / BYTES_PER_PAGE; 
  unsigned int linesPerPage  = 0; // BYTES_PER_PAGE / BYTES_PER_LINE;
  unsigned int linesPerChain = 0; // linesPerPage * pagesPerChain;
  unsigned int linksPerLine  = 0; // BYTES_PER_LINE / sizeof(Chain);
  unsigned int linksPerChain = 0; // linesPerChain * linksPerLine;
  /*
  unsigned long memLatency   = 0;
  unsigned long memBandWidth = 0; 
  */
  // Chain linked list
  pChain root = NULL, prev = NULL, cur = NULL;
  int link = 0;
  // Chase Memory
  pChain chain_memory = malloc(sizeof(Chain) * linksPerChain);

  printf("Starting to pChase benchmark\n");
  for (j = 0; j < NUM_OF_EXPRIMENTS; j++) {
    for (k = 0; k < NUM_OF_EXPRIMENTS; k++) {
      // calculate performance benchmark parameters
      pagesPerChain = CHAIN_SIZE / BYTES_PER_PAGE[k];
      linesPerPage  = BYTES_PER_PAGE[k] / BYTES_PER_LINE[j];
      linesPerChain = linesPerPage * pagesPerChain;
      linksPerLine  = BYTES_PER_LINE[j] / sizeof(Chain);
      linksPerChain = linesPerChain * linksPerLine; 

      printf("  Case_%d\n", j*NUM_OF_EXPRIMENTS+(k+1));
      printf("  -- Chain Memory size = %d (B)\n", CHAIN_SIZE);
      printf("  -- Page entry size   = %d (Bytes)\n", BYTES_PER_LINE[j]);
      printf("  -- Page table size   = %d (Bytes)\n", BYTES_PER_PAGE[k]);

      // initialize xtimer
      xtimer_init();
   
      // start time and time unit is microsecond(us)
      startTime = xtimer_now();

      // Linked list by point to chain_memory
      for(i = 0; i < linesPerChain ; i++) {
        link = i * linksPerLine;
        prev = malloc(sizeof(Chain));
        if (root == NULL) {
          prev = root = chain_memory + link;
        } else {
          prev->_next = chain_memory + link;
          prev = prev->_next;
        }
        numOfOperation++;
      }
      prev->_next = NULL; // last node

      // Chase whole memory by pointer
      for(i = 0; i < NUM_OF_ITERATION; i++) {
        cur = root;
        // search whole pointed memory by while and check pointer
        while(cur != NULL) {
          prev = cur;
          cur = cur->_next;
        }
      }

      // end time
      //endTime = clock_seconds();
      endTime = xtimer_now();

      // TARGET=z1 cannot print double or float type, only shown by integer
      printf("  ... Finished\nSummary: \n");
      printf("  Start time       = %lu  (us)\n", startTime);
      printf("  End time         = %lu  (us)\n", endTime);
      printf("  Elapsed time     = %lu  (us)\n", endTime - startTime);
      printf("  NumOfOperation   = %u\n", numOfOperation);
      printf("  NumOfIteration   = %d\n\n", NUM_OF_ITERATION);
      /*   
      // use Excel to calculate the result because cooja cannot print float/double data
      memLatency = ((unsigned long)((endTime - startTime) * US) / (numOfOperation * NUM_OF_ITERATION));
      memBandWidth = ((unsigned long)(numOfOperation * NUM_OF_ITERATION * BYTES_PER_LINE * US) / (endTime-startTime));
      printf("memory latency   = %lu (us)\n", memLatency);
      printf("memory bandwidth = %lu (B/s)\n", memBandWidth);
      */
      free(prev);
    }
  }
  // free memory
  free(chain_memory);
  printf("pChase benchmark finished! Thank you \n");
  return 0;
} 
/*---------------------------------------------------------------------------*/
