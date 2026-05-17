#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h> 

#include "error.h"
#include "debug.h"

#include "bank.h"
#include "branch.h"
#include "account.h"
#include "report.h"

#define MAX_NUM_REPORTS 8       
#define MAX_LOG_ENTRIES 1024    

typedef struct Report {
  int numReports;          
  struct {                 
    AccountAmount balance; 
    int hasOverflowed;     
    int numLogEntries;     
    struct TransferLog {                
      AccountNumber accountNum;
      AccountAmount transferSize;
    } transferLog[MAX_LOG_ENTRIES];
  } dailyData[MAX_NUM_REPORTS];
} Report;

static AccountAmount reportingAmount;   
static int numWorkers;                  

pthread_mutex_t report_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t all_here_cv = PTHREAD_COND_INITIALIZER;
int workers_arrived = 0;

// ------------------------------

/*
 * Initialize the Report module of a bank.
 */
int
Report_Init(Bank *bank, AccountAmount reportAmount, int maxNumWorkers)
{
  bank->report = (Report *) malloc(sizeof(Report));
  if (bank->report == NULL) {
    return -1;
  }

  bank->report->numReports = 0;

  for (int r = 0; r < MAX_NUM_REPORTS; r++) {
    bank->report->dailyData[r].hasOverflowed = 0;
    bank->report->dailyData[r].numLogEntries = 0;
  }

  reportingAmount = reportAmount;
  numWorkers = maxNumWorkers;

  return 0;
}

/*
 * Report a transfer. Needs locking because multiple threads call it.
 */
int
Report_Transfer(Bank *bank, int workerNum, AccountNumber accountNum,
                AccountAmount amount)
{
  AccountAmount amountAbs = (amount < 0) ? -amount : amount;
  if (amountAbs < reportingAmount) {
    return 0;  
  }

  pthread_mutex_lock(&report_lock);

  Report *rpt = bank->report;
  int r  = rpt->numReports;

  if (r >= MAX_NUM_REPORTS) {
      pthread_mutex_unlock(&report_lock);
      return 0;
  }

  if (rpt->dailyData[r].numLogEntries >= MAX_LOG_ENTRIES) {
    rpt->dailyData[r].hasOverflowed = 1;
    pthread_mutex_unlock(&report_lock);
    return 0;
  }

  int ent = rpt->dailyData[r].numLogEntries;
  rpt->dailyData[r].transferLog[ent].accountNum = accountNum;
  rpt->dailyData[r].transferLog[ent].transferSize = amount;   
  rpt->dailyData[r].numLogEntries = ent + 1;

  pthread_mutex_unlock(&report_lock);

  return 0;
}

/*
 * Perform the nightly report. This acts as a BARRIER.
 */
int
Report_DoReport(Bank *bank, int workerNum)
{
  Report *rpt = bank->report;
  assert(rpt);

  pthread_mutex_lock(&report_lock);

  workers_arrived += 1;

 if (workers_arrived >= numWorkers) {
    
    if (rpt->numReports < MAX_NUM_REPORTS) {
        pthread_mutex_unlock(&report_lock);  // ← ჯერ გაათავისუფლე!
        
        Bank_Balance(bank, &rpt->dailyData[rpt->numReports].balance);
        
        pthread_mutex_lock(&report_lock);    // ← მერე კიდევ lock
        rpt->numReports += 1;
    }
    
    workers_arrived = 0;
    pthread_cond_broadcast(&all_here_cv);
} else {
      
      int lastReportID = rpt->numReports;

      while (rpt->numReports == lastReportID) {
          pthread_cond_wait(&all_here_cv, &report_lock);
      }
      
  }

  pthread_mutex_unlock(&report_lock);

  return 0;
}

/*
 * Sorting function (unchanged logic)
 */
static int
TransferLogSortFunc(const void *p1, const void *p2)
{
  const struct TransferLog *l1 = (const struct TransferLog *) p1;
  const struct TransferLog *l2 = (const struct TransferLog *) p2;

  if (l1->accountNum < l2->accountNum) return -1;
  if (l1->accountNum > l2->accountNum) return 1;
  if (l1->transferSize < l2->transferSize) return -1;
  if (l1->transferSize > l2->transferSize) return 1;

  return 0;
}

/*
 * Compare reports (unchanged logic, logic doesn't need locks as it runs single-threaded at the end)
 */
int
Report_Compare(Bank *bank1, Bank *bank2)
{
  int err = 0;
  Report *rpt1 = bank1->report;
  Report *rpt2 = bank2->report;

  if (rpt1->numReports != rpt2->numReports) {
    fprintf(stderr, "Bank num reports mismatch %d != %d\n",
            rpt1->numReports, rpt2->numReports);
    err = -1;
  }

  for (int r = 0; r < rpt1->numReports; r++) {
    if (rpt1->dailyData[r].balance != rpt2->dailyData[r].balance) {
      fprintf(stderr, "Report %d for banks mismatch %"PRId64" and %"PRId64"\n",
              r, rpt1->dailyData[r].balance, rpt2->dailyData[r].balance);
      err = -1;
    }
    if (rpt1->dailyData[r].numLogEntries !=  rpt2->dailyData[r].numLogEntries) {
      fprintf(stderr, "Report different number of log entries (%d and %d)\n",
              rpt1->dailyData[r].numLogEntries,
              rpt2->dailyData[r].numLogEntries);
      return -1;
    }
    if (!rpt1->dailyData[r].hasOverflowed) {
      int i, n;
      n = rpt1->dailyData[r].numLogEntries;
      qsort(rpt1->dailyData[r].transferLog, n, sizeof(struct TransferLog),
            TransferLogSortFunc);

      assert(n == rpt2->dailyData[r].numLogEntries);
      qsort(rpt2->dailyData[r].transferLog, n, sizeof(struct TransferLog),
            TransferLogSortFunc);

      for (i = 0; i < n; i++) {
        if ((rpt1->dailyData[r].transferLog[i].accountNum !=
             rpt2->dailyData[r].transferLog[i].accountNum) ||
            (rpt1->dailyData[r].transferLog[i].transferSize !=
             rpt2->dailyData[r].transferLog[i].transferSize)) {
          fprintf(stderr, "Report transferLog %d difference at %d\n", r, i);
          err = -1;
        }
      }
    }
  }
  return err;
}