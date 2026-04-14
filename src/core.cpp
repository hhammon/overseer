#include "core.hpp"

template <typename T>
T& View<T>::operator[](u64 index) {
	assert(index < this->len);

	return this->ptr[index];
}