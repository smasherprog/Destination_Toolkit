#ifndef CSETARRAY_H
#define CSETARRAY_H

template <class T, class U>struct KdNode {
	KdNode <T, U> *lower, *upper;
	U index;
	T *point;
};

template <class T, class U>class cSetArray {
public:

	KdNode <T, U> *top;
	size_t Size, Capacity, nComponents;
	unsigned char *mem, *curr;

	cSetArray(const size_t ncomp, const size_t capacity):top(0), Size(0), Capacity(capacity), nComponents(ncomp) {
		curr = mem = (unsigned char *) malloc(capacity * (sizeof(KdNode <T, U>) + nComponents * sizeof(T)));
	}
	~cSetArray(){ free(mem); }
	U insert(const T* point){
		if (top != NULL) return insert(top, point);
		else top = newNode(point);// otherwise, create new top node and return 0 as the first index
		return 0;
	}
	void clear(){// does not free the memory, just resets 
		curr = mem;
		top = NULL;
		Size = 0;
	}
	const size_t size() const { return Size; }// the current size
	U insert(KdNode <T, U> *node, const T* point){
		size_t comp = 0;
		while (true){
			if (point[comp] < node->point[comp]){
				if (node->lower){
					node = node->lower;
				} else {
					node->lower = newNode(point);
					return node->lower->index;
				}
			} else if (point[comp] > node->point[comp]){
				if (node->upper){
					node = node->upper;
				} else {
					node->upper = newNode(point);
					return node->upper->index;
				}
			} else if (isEqualToNode(node, point)){
				return node->index;
			} else {
				if (node->upper){
					node = node->upper;
				} else {
					node->upper = newNode(point);
					return node->upper->index;
				}
			}
			if (++comp == nComponents) comp = 0;// should never be true.. 
		}
	}

	KdNode <T, U> *newNode(const T *point){
		KdNode <T, U> *node = (KdNode <T, U> *) newMem(sizeof(KdNode <T, U>));
		node->point = (T*) newMem(nComponents * sizeof(T));
		memcpy(node->point, point, nComponents * sizeof(T));
		node->lower = NULL;
		node->upper = NULL;
		node->index = static_cast<U>(Size++);
		return node;
	}
	bool isEqualToNode(const KdNode <T, U> *node, const T *point){
		unsigned int i = 0;
		do {
			if (node->point[i] != point[i]) return false;
		} while (++i < nComponents);
		return true;
	}

	void *newMem(const size_t size){
		unsigned char *rmem = curr;
		curr += size;
		return rmem;
	}

};

#endif 
