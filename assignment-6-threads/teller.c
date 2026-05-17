#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"

/*
 * Teller_DoDeposit
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  pthread_mutex_lock(&account->lock);
  account->balance += amount;
  pthread_mutex_unlock(&account->lock);

  int branchID = AccountNum_GetBranchID(accountNum);

  pthread_mutex_lock(&bank->branches[branchID].lock);
  bank->branches[branchID].balance += amount;
  pthread_mutex_unlock(&bank->branches[branchID].lock);

  return ERROR_SUCCESS;
}

/*
 * Teller_DoWithdraw
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  pthread_mutex_lock(&account->lock);

  if (amount > account->balance) {
    pthread_mutex_unlock(&account->lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }

  account->balance -= amount;
  pthread_mutex_unlock(&account->lock);

  int branchID = AccountNum_GetBranchID(accountNum);

  pthread_mutex_lock(&bank->branches[branchID].lock);
  bank->branches[branchID].balance -= amount;
  pthread_mutex_unlock(&bank->branches[branchID].lock);

  return ERROR_SUCCESS;
}

/*
 * Teller_DoTransfer
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) return ERROR_ACCOUNT_NOT_FOUND;

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) return ERROR_ACCOUNT_NOT_FOUND;

  if (srcAccount == dstAccount) return ERROR_SUCCESS;


  if (srcAccount->accountNumber < dstAccount->accountNumber) {
    pthread_mutex_lock(&srcAccount->lock);
    pthread_mutex_lock(&dstAccount->lock);
  } else {
    pthread_mutex_lock(&dstAccount->lock);
    pthread_mutex_lock(&srcAccount->lock);
  }

  if (amount > srcAccount->balance) {
    pthread_mutex_unlock(&srcAccount->lock);
    pthread_mutex_unlock(&dstAccount->lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }

  
  srcAccount->balance -= amount;
  
  dstAccount->balance += amount;


  pthread_mutex_unlock(&srcAccount->lock);

  pthread_mutex_unlock(&dstAccount->lock);




  int srcID = AccountNum_GetBranchID(srcAccountNum);
  
  int dstID = AccountNum_GetBranchID(dstAccountNum);

  if (srcID != dstID) {
      if (srcID < dstID) {
          pthread_mutex_lock(&bank->branches[srcID].lock);
          pthread_mutex_lock(&bank->branches[dstID].lock);
      } else {
          pthread_mutex_lock(&bank->branches[dstID].lock);
          pthread_mutex_lock(&bank->branches[srcID].lock);
      }

      bank->branches[srcID].balance = bank->branches[srcID].balance - amount;
      bank->branches[dstID].balance =  bank->branches[dstID].balance + amount;



     if (srcID < dstID) {
          pthread_mutex_unlock(&bank->branches[srcID].lock);
          pthread_mutex_unlock(&bank->branches[dstID].lock);
      } else {
          pthread_mutex_unlock(&bank->branches[dstID].lock);
          pthread_mutex_unlock(&bank->branches[srcID].lock);
      }
  }

  return ERROR_SUCCESS;
}