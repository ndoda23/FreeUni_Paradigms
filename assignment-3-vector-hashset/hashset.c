#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize>0);
	assert(comparefn != NULL);
	assert(hashfn != NULL);
	assert(numBuckets > 0);

	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->logicalLength = 0;
	h->hashFn = hashfn;
	h->compareFn = comparefn;
	h->freeFn = freefn;
	h->buckets = malloc(h->numBuckets*sizeof(vector));

	for(int i = 0 ; i < numBuckets;i++){
		VectorNew(&h->buckets[i],elemSize,freefn,4);
	}

}

void HashSetDispose(hashset *h)
{
	for (int i = 0; i < h->numBuckets; i++)
	{
		VectorDispose(&h->buckets[i]);
	}
	free(h->buckets);
	
}

int HashSetCount(const hashset *h)
{ 
	return h->logicalLength; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn!=NULL);

	for(int i = 0 ;i < h->numBuckets;i++){
		VectorMap(&h->buckets[i],mapfn,auxData);
	}

}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr!=NULL);
	int bucketNumber = h->hashFn(elemAddr,h->numBuckets);
	assert(bucketNumber >= 0 && bucketNumber < h->numBuckets);
	int num = VectorSearch(&h->buckets[bucketNumber],elemAddr,h->compareFn,0,false);
	if(num==-1){
		VectorAppend(&h->buckets[bucketNumber],elemAddr);
		h->logicalLength++;
		return;
	}else{
		VectorReplace(&h->buckets[bucketNumber],elemAddr,num);
		return;
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int bucketNumber = h->hashFn(elemAddr,h->numBuckets);
	assert(bucketNumber >= 0 && bucketNumber < h->numBuckets);

	int idx = VectorSearch(&h->buckets[bucketNumber],elemAddr,h->compareFn,0,false);
	if(idx==-1){
		return NULL;
	}

	return VectorNth(&h->buckets[bucketNumber],idx);
}
