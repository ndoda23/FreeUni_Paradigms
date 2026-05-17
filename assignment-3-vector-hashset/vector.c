#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define GET_ELEM(i)  ((void*)((char*)v->base + (i)*v->elemSize))

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(initialAllocation >= 0);
  v->elemSize = elemSize;
  v->freeFn = freeFn;
  v->logLength = 0;
  v->capacity = (initialAllocation == 0) ? 4 : initialAllocation;
  v->base = malloc(v->elemSize*v->capacity);
  assert(v->base);

}

void VectorDispose(vector *v)
{
   if(v->freeFn!=NULL){
    for(int i = 0 ; i < v->logLength;i++){
      void* elem = GET_ELEM(i);
      v->freeFn(elem);
    }
   }

   free(v->base);
}

int VectorLength(const vector *v)
{ return v->logLength; }

void *VectorNth(const vector *v, int position)
{  
   assert(position >= 0);
   assert(position < v->logLength);
   return GET_ELEM(position);
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert(position>=0 && position < v->logLength);
  assert(elemAddr);
  if(v->freeFn != NULL){
    v->freeFn(GET_ELEM(position));
  }
  memcpy(GET_ELEM(position),elemAddr,v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position>=0 && position < v->logLength+1);
  assert(elemAddr);
  if(v->logLength == v->capacity){
    v->capacity = (v->capacity == 0) ? 4 : v->capacity * 2;
    v->base = realloc(v->base,v->capacity*v->elemSize);
  }

  if(v->logLength == position){
    memcpy(GET_ELEM(position),elemAddr,v->elemSize);
    v->logLength++;
  }else{
    memmove(GET_ELEM(position+1),GET_ELEM(position),v->elemSize*(v->logLength-position));
    memcpy(GET_ELEM(position),elemAddr,v->elemSize);
    v->logLength++;
  }
}



void VectorAppend(vector *v, const void *elemAddr)
{
  assert(elemAddr);
  if(v->capacity == v->logLength){
    v->capacity = (v->capacity == 0) ? 4 : v->capacity * 2;
    v->base = realloc(v->base,v->elemSize*v->capacity);
  }
  memcpy(GET_ELEM(v->logLength),elemAddr,v->elemSize);
  v->logLength++;
}

void VectorDelete(vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  void* el = GET_ELEM(position);
  if(v->freeFn){
    v->freeFn(el);
  }
  memmove(GET_ELEM(position),GET_ELEM(position+1),v->elemSize*(v->logLength-position-1));
  v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  if(!compare) {
    return;
  }  

  qsort(v->base,v->logLength,v->elemSize,compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  if(!mapFn){
    return;
  }

  for(int i = 0 ; i < v->logLength;i++){
    mapFn(GET_ELEM(i),auxData);
  }

}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    assert(startIndex >= 0 && startIndex <= v->logLength);
    assert(searchFn!=NULL);
    assert(key!=NULL);
    if(isSorted){
      void* elem = bsearch(key,GET_ELEM(startIndex),v->logLength-startIndex,v->elemSize,searchFn);
      if(elem!=NULL){
        return ((char*)elem - (char*)v->base)/v->elemSize;
      }
    }else{
      for(int i = startIndex ; i < v->logLength;i++){
        if(!searchFn(key,GET_ELEM(i))){
          return i;
        }
      }
    }
    
    return kNotFound;
} 
