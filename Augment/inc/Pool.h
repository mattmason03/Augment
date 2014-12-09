#pragma once

class Pool{
public:
	Pool(size_t numElements, size_t sizeElements);

	char* Get(size_t index);

	const size_t eltNum;
	const size_t eltSize;
	char* memory;
};

template <size_t TEltCt, typename TComp>
class CompPool : public Pool{
public:
	CompPool<TEltCt, TComp>();
};

Pool::Pool(size_t numElements, size_t sizeElements)
	:eltNum(numElements), eltSize(sizeElements)
{
	memory = new char[eltNum * eltSize];
}

char* Pool::Get(size_t index){
	return (memory + index * eltSize);
}

template <size_t TEltCt, typename TComp>
CompPool<TEltCt, TComp>::CompPool()
	:Pool(TEltCt, sizeof(TComp::Type))
{}