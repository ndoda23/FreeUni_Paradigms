#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "error.h"
#include "bank.h"
#include "branch.h"
#include "account.h"
#include "report.h"

/*
 * allocate the bank structure and initialize the branches.
 */
/*
 * allocate the bank structure and initialize the branches.
 */
Bank*
Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
          AccountAmount reportingAmount,
          int maxNumWorkers)
{
  Bank *bank = (Bank *) malloc(sizeof(Bank));
  if (bank == NULL) {
    return NULL;
  }

  int branchStatus = Branch_Init(bank, numBranches, numAccounts, initAmount);
  
  if (branchStatus != 0) {
    free(bank);
    return NULL;
  }

  int reportStatus = Report_Init(bank, reportingAmount, maxNumWorkers);
  
  if (reportStatus != 0) {
    free(bank);
    return NULL;
  }

  return bank;
}

/*
 * get the balance of the entire bank by adding up all the balances in
 * each branch.
 */
int
Bank_Balance(Bank *bank, AccountAmount *balance)
{
  assert(bank->branches);

  AccountAmount bankTotal = 0;

  for (unsigned int i = 0; i < bank->numberBranches; i++) {
      pthread_mutex_lock(&bank->branches[i].lock);
  }

  for (unsigned int i = 0; i < bank->numberBranches; i++) {
      AccountAmount branchBal;
      branchBal = bank->branches[i].balance;
      bankTotal += branchBal;
  }

  for (unsigned int i = 0; i < bank->numberBranches; i++) {
      pthread_mutex_unlock(&bank->branches[i].lock);
  }

  *balance = bankTotal;

  return 0;
}

/*
 * tranverse and validate each branch.
 */
int
Bank_Validate(Bank *bank)
{
  assert(bank->branches);
  int err = 0;

  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    int berr = Branch_Validate(bank, branch);
    if (berr < 0) {
      err = berr;
    }
  }
  return err;
}

/*
 * compare two banks
 */
int
Bank_Compare(Bank *bank1, Bank *bank2)
{
  int err = 0;
  if (bank1->numberBranches != bank2->numberBranches) {
    fprintf(stderr, "Bank num branches mismatch\n");
    return -1;
  }

  for (unsigned int i = 0; i < bank1->numberBranches; i++) {
    int bErr = Branch_Compare(&bank1->branches[i], &bank2->branches[i]);
    if (bErr) {
      err = -1;
    }
  }

  int rErr = Report_Compare(bank1, bank2);
  if (rErr) {
    err = -1;
  }

  return err;
}