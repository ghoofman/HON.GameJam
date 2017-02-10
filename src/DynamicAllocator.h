#pragma once
#include "./OPengine.h"

template<class T>
class DynamicAllocator {
	T* data;
	ui32 curr;
	ui32 step;
	ui32 maxCount;

public:
	DynamicAllocator(ui32 initial)
	{
		step = initial;
		maxCount = initial;
		curr = 0;
		data = OPNEW(T[maxCount]);
	}

	inline void Destroy() {
		OPfree(data);
	}

	inline void Free() {
		Destroy();
		OPfree(this);
	}

	inline T* Next() {
		if (curr + 1 == maxCount) {
			maxCount += step;
			data = (T*)OPrealloc(data, sizeof(T) * maxCount);
		}
		return &data[curr++];
	}

	inline T* Get(ui32 i) {
		return &data[i];
	}

	inline ui32 Count() {
		return curr;
	}

	inline T* Remove(ui32 i) {
		curr--;
		if (i == curr) {
			return NULL;
		}
		data[i] = data[curr];
		return &data[i];
	}

	inline T* Remove(T* el) {
		for (ui32 i = 0; i < curr; i++) {
			if (&data[i] == el) {
				return Remove(i);
			}
		}
		return NULL;
	}
};