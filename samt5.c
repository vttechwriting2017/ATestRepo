#include "CSet.h"

#include "stdlib.h"

// CSet provides an implementation of a set type for storing a collection of
// signed 32-bit integer values (int32_t).
//
// The implementation imposes the following constraints:
//  - storage is array-based
//  - duplicate elements are not allowed in a CSet
//  - logically empty cells are set to INT32_MIN
//  - sets can contain up to UINT32_MAX elements
//  - unless noted to the contrary, the worst-case cost of each operation
//    is O(N), where N is the number of elements in the CSet object(s)
//    that are involved
//  - empty test is O( 1 )
//  - Contains() is O( log N )
//  - there are no memory leaks during any of the supported operations
//
// We say a CSet object A is proper if and only if it satisfies each of the 
// following conditions:
//
//  1.  If A.Capacity == 0 then A.Usage == 0 and A.Data == NULL.
//  2.  If A.Capacity > 0 then A.Data points to an array of dimension A.Capacity.
//  3.  A.Data[0 : A.Usage-1] are the values stored in the set 
//      (in unspecified order)
//  4.  A.Data[A.Usage : A.Capacity-1] equal INT32_MIN (FILLER)
//
// CSet objects that have not been initialized are said to be raw.
//
// With the sole exception of CSet_Init(), all CSet objects supplied as
// parameters are expected to be proper.
//
// This is here for demo purposes

/**
 * Initializes a raw pSet object, with capacity Sz.
 *
 * Pre:
 *    pSet points to a CSet object, but *pSet may be proper or raw
 *    Sz   has been initialized
 * Post:  
 *    If successful:
 *       pSet->Capacity == Sz
 *       pSet->Usage == 0
 *       pSet->Data points to an array of dimension Sz, or
 *          is NULL if Sz == 0
 *       if Sz != 0, pSet->Data[0 : Sz-1] == INT32_MIN
 *    else:
 *       pSet->Capacity == 0
 *       pSet->Usage == 0
 *       pSet->Data == NULL
 * Returns:
 *    true if successful, false otherwise
 * 
 * Complexity:  O( Sz )
 */
bool CSet_Init(CSet* const pSet, uint32_t Sz) {
	//*pSet = (struct CSet*)malloc(sizeof(struct _CSet));
	if (pSet == NULL) return false;
	pSet->Capacity = Sz;
	pSet->Usage = 0;
	if (Sz > 0) {
		pSet->Data = (int32_t*)malloc(Sz * sizeof(int32_t));
		if (pSet->Data == NULL) {
			pSet->Capacity = 0;
			return false;
		}
	}
	else {
		pSet->Data = NULL;
	}
	uint32_t i = 0;
	while (Sz > 0 && i < Sz) {
		pSet->Data[i] = INT32_MIN;
		i++;
	}
	return true;
}

/**
 * Adds Value to a pSet object.
 *
 * Pre:
 *    *pSet is proper
 *    Value has been initialized
 * Post:  
 *    If successful:
 *       Value is a member of *pSet
 *       pSet->Capacity has been increased, if necessary
 *       *pSet is proper
 *    else:
 *       *pSet is unchanged
 * Returns:
 *    true if successful, false otherwise
 * 
 * Complexity:  O( pSet->Usage )
 */
bool CSet_Insert(CSet* const pSet, int32_t Value) {
	if (pSet->Data == NULL) {
		return false;
	}
	//Determine if we have enough space to insert a value
	if (pSet->Usage + 1 < pSet->Capacity) {
		uint32_t i = 0;
		while (i < pSet->Usage) {
			if (pSet->Data[i] == Value) {
				return false;
			}
			else if (pSet->Data[i] < Value) {
				//Not here
				i++;
			}
			else {
				//We found where we want it.
				break;
			}
		}
		uint32_t j = pSet->Usage + 1;
		while (i < j) {
			pSet->Data[j] = pSet->Data[j - 1];
			j--;
		}
		pSet->Data[i] = Value;
	}
	//If we don't have space, make a new array and move everything there
	else {
		uint32_t capacity = pSet->Capacity;
		if (capacity <= 0) {
			capacity = 1;
		}
		int32_t* NewData = malloc(capacity * 2 * sizeof(int32_t));
		if (NewData == NULL) { return false; }
		pSet->Capacity = capacity * 2;
		uint32_t i = 0;
		uint32_t j = 0;
		while (i < pSet->Usage + 1) {
			if (j < pSet->Usage && pSet->Data[j] < Value) {
				NewData[i] = pSet->Data[j];
				j++;
			}
			else {
				NewData[i] = Value;
			}
			i++;
		}
		while (i < pSet->Capacity) {
			NewData[i] = INT32_MIN;
			i++;
		}
		free(pSet->Data);
		pSet->Data = NewData;
	}
	pSet->Usage++;
	return true;
}

/**
 * Removes Value from a pSet object.
 *
 * Pre:
 *    *pSet is proper
 *    Value has been initialized
 * Post:  
 *    If Value was a member of *pSet:
 *       Value is no longer a member of *pSet
 *       pSet->Capacity is unchanged
 *       pSet->Usage is decremented
 *       *pSet is proper
 *    else:
 *       *pSet is unchanged
 * Returns:
 *    true if Value was removed, false otherwise
 * 
 * Complexity:  O( pSet->Usage )
 */
 bool CSet_Remove(CSet* const pSet, int32_t Value) {
	bool found = false;
	uint32_t i = 0;
	//Iterate through until we find it or we reach the end of the pSet
	while (i < pSet->Usage) {
		if (pSet->Data[i] == Value) {
			//We found it
			found = true;
			break;	
		}
		//Not found yet, going again
		i++;
	}
	//If we found it remove it by moving the next value on top
	if (found && i == pSet->Usage - 1) {
		pSet->Data[i] = INT32_MIN;
		pSet->Usage--;
	}
	else if (found) {
		while (i <= pSet->Usage - 2) {
			pSet->Data[i] = pSet->Data[i + 1];
			i++;
		}
		pSet->Usage--;
		CSet_Remove(pSet, Value);
		pSet->Data[pSet->Usage] = INT32_MIN;
	}
	return found;
}

/**
 * Determines if Value belongs to the given pSet object.
 *
 * Pre:
 *    *pSet is proper
 *    Value has been initialized
 * Post:  
 *    *pSet is unchanged
 * Returns:
 *    true if Value belongs to *pSet, false otherwise
 * 
 * Complexity:  O( log(pSet->Usage) )
 */
bool CSet_Contains(const CSet* const pSet, int32_t Value) {
	if (pSet->Usage == 0) return false;
	int32_t max = pSet->Usage - 1;
	int32_t min = 0;
	//Binary search for the max value
	int32_t mid = (min + max) / 2;
	while (min <= max) {
		//Did we find it?
		//printf("min: %d\nmax: %d\nmid: %d\n", min, max, mid);
		if (pSet->Data[mid] == Value) {
			return true;
		}
		//Is the current element larger than Value?
		if (pSet->Data[mid] > Value) {
			max = mid - 1;
		}
		//Is it smaller?
		else if (pSet->Data[mid] < Value) {
			min = mid + 1;
		}
		mid = (min + max) / 2;
	}
	return false;
}

/**
 * Determines if two CSet objects contain the same elements.
 *
 * Pre:
 *    *pA and *pB are proper
 * Post:  
 *    *pA is unchanged
 *    *pB is unchanged
 * Returns:
 *    true if sets contain same elements, false otherwise
 * 
 * Complexity:  O( pA->Usage )
 */
bool CSet_Equals(const CSet* const pA, const CSet* const pB) {
	if (pA->Usage != pB->Usage) {
		return false;
	}
	uint32_t i = 0;
	//We know they must have the same order because they are both sorted.
	//Look at each element and determine if they are the same
	while (i < pA->Usage) {
		if (pA->Data[i] != pB->Data[i]) {
			return false;
		}
		i++;
	}
	return true;
}

/**
 * Determines if one CSet object is a subset of another.
 *
 * Pre:
 *    *pA and *pB are proper
 * Post:  
 *    *pA is unchanged
 *    *pB is unchanged
 * Returns:
 *    true if *pB contains every element of *pA, false otherwise
 * Complexity:  O( pA->Usage )
 */
bool CSet_isSubsetOf(const CSet* const pA, const CSet* const pB) {
	if (pA->Usage > pB->Usage) {
		//pB can't have all elements in pA if |pA| > |pB|
		return false;
	}
	uint32_t a = 0;
	uint32_t b = a;
	while (a < pA->Usage && b < pB->Usage) {
		if (pA->Data[a] > pB->Data[b]) {
			a++;
			if (b == pB->Usage - 1) return false;
		}
		else if (pA->Data[a] == pB->Data[b]) {
			b++;
			a++;
		}
		else {
			return false;
		}
	}
	return !(a < pA->Usage);
}

/**
 * Sets *pIntersection to be the intersection of the sets *pA and *pB.
 *
 * Pre:
 *    *pIntersection is proper
 *    *pA            is proper
 *    *pB            is proper
 * Post:
 *    *pA and *pB are unchanged, unless *pIntersection aliases *pA or *pB
 *    For every integer x, x is contained in *pIntersection iff x is contained
 *       in both *pA and *pB.
 *    pIntersection->Capacity == min(pA->Capacity, pB->Capacity)
 *    pIntersection->Usage    == number of elements that occur in
 *       both *pA and *pB
 *    *pIntersection is proper
 * Returns:
 *    true if the intersection is successfully created; false otherwise
 * 
 * Complexity:  O( max(pA->Usage, pB->Usage) )
 */
bool CSet_Intersection(CSet* const pIntersection, const CSet* const pA, const CSet* const pB) {
	uint32_t i = 0;
	uint32_t a = 0;
	uint32_t b = 0;
	uint32_t capacity = (pA->Capacity > pB->Capacity) ? pB->Capacity : pA->Capacity;
	int32_t* data = (int32_t*)malloc(capacity * sizeof(int32_t));
	if (data == NULL) {
		return false;
	}
	while (a < pA->Usage && b < pB->Usage) {
		if (pA->Data[a] < pB->Data[b]) {
			a++;
		}
		else if (pA->Data[a] > pB->Data[b]) {
			b++;
		}
		else {
			data[i] = pA->Data[a];
			a++;
			i++;
		}
	}
	pIntersection->Usage = i;
	while (i < capacity) {
		data[i] = INT32_MIN;
		i++;
	}
	free(pIntersection->Data);
	pIntersection->Data = data;
	pIntersection->Capacity = capacity;
	return true;
}
 
/**
 * Sets *pSym to be the symmetric difference of the sets *pA and *pB.
 *
 * Pre:
 *    *pSym, *pA and *pB are proper
 * Post:
 *    *pA and *pB are unchanged
 *    For every integer x, x is contained in *pSym iff x is contained in
 *       *pA but not in *pB, or x is contained i *pB but not in *pA.
 *    pDiff->Capacity == pA->Capacity + pB->Capacity
 *    pDiff->Usage    == pA->Usage - number of elements that
 *                        occur in exactly one of *pA and *pB
 *    *pSym is proper
 * Returns:
 *    true if the difference is successfully created; false otherwise
 * 
 * Complexity:  O( max(pA->Usage, pB->Usage) )
 */
bool CSet_SymDifference(CSet* const pSym, const CSet* const pA, const CSet* const pB) {
	uint32_t capacity = pA->Capacity + pB->Capacity;
	int32_t* data = (int32_t*)malloc(capacity * sizeof(int32_t));
	if (data == NULL) return false;
	uint32_t i = 0;
	uint32_t a = 0;
	uint32_t b = 0;
	while (a < pA->Usage || b < pB->Usage) {
		if (a < pA->Usage && b < pB->Usage) {
			if (pA->Data[a] == pB->Data[b]) {
				a++;
				b++;
			}
			else if (pA->Data[a] < pB->Data[b]) {
				data[i] = pA->Data[a];
				i++;
				a++;
			}
			else {
				data[i] = pB->Data[b];
				i++;
				b++;
			}
		}
		//If b is at its usage then all in a should be added
		else if (a < pA->Usage) {
			data[i] = pA->Data[a];
			i++;
			a++;
		}
		//Inverse of previous conditional statement.
		else {
			data[i] = pB->Data[b];
			i++;
			b++;
		}
	}
	pSym->Usage = i;
	while (i < capacity) {
		data[i] = INT32_MIN;
		i++;
	}
	if (pSym->Data != NULL) {
		free(pSym->Data);
	}
	pSym->Data = data;
	pSym->Capacity = capacity;
	return true;
}

/**
 * Makes a deep copy of a CSet object.
 *
 * Pre:
 *    *pTarget and *pSource are proper
 * Post:  
 *    *pSource is unchanged
 *    If successful:
 *       pTarget->Capacity == pSource->Capacity
 *       pTarget->Usage == pSource->Usage
 *       pTarget[0:pTarget->Capacity-1] ==  pSource[0:pSource->Capacity-1]
 *       pTarget->Data != pSource->Data, unless pTarget == pSource
 *       *pTarget is proper.
 *    else:
 *       *pTarget is unchanged
 * Returns:
 *    true if successful, false otherwise
 * 
 * Complexity:  O( max(pSource->Usage) )
 */
bool CSet_Copy(CSet* const pTarget, const CSet* const pSource) {
	if (pTarget == pSource) return true;
	int32_t* initialData = NULL;
	if (pTarget->Data != NULL) {
		initialData = pTarget->Data;
	}
	pTarget->Data = malloc(pSource->Capacity * sizeof(int32_t));
	if (pTarget->Data == NULL) {
		pTarget->Data = initialData;
		return false;
	}
	for (uint32_t i = 0; i < pSource->Capacity; i++) {
		pTarget->Data[i] = pSource->Data[i];
	}
	free(initialData);
	pTarget->Usage = pSource->Usage;
	pTarget->Capacity = pSource->Capacity;
	return true;
}

/**
 *  Reports the current capacity of a pSet object.
 *
 *  Pre:
 *     *pSet is proper
 *  Post:
 *     *pSet is unchanged
 *  Returns:
 *     pSet->Capacity
 * 
 * Complexity:  O( 1 )
 */
uint32_t CSet_Capacity(const CSet* const pSet) {
	return pSet->Capacity;
}

/**
 *  Reports the number of elements in a pSet object.
 *
 *  Pre:
 *     *pSet is proper
 *  Post:
 *     *pSet is unchanged
 *  Returns:
 *     pSet->Usage
 * 
 * Complexity:  O( 1 )
 */
uint32_t CSet_Usage(const CSet* const pSet) {
	return pSet->Usage;
}

/**
 *  Determines whether a CSet object is empty.
 *
 *  Pre:
 *     *pSet is proper
 *  Post:
 *     *pSet is unchanged
 * Complexity:  O( 1 )
 */
bool CSet_isEmpty(const CSet* const pSet) {
	return (pSet->Usage == 0);
}

