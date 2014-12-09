#include "EntityFramework.h"

unsigned short BaseComponent::componentCounter = 0;

template <typename T>
unsigned short Component<T>::componentId = USHRT_MAX;

template <typename T>
T& Component<T>::operator->(){
	return data;
}

template <typename T>
void Component<T>::Register(){
	componentId = ++componentCounter;
}

template <typename... T>
EntityIter<T...>::EntityIter(EntityFramework ef, Component<T>& ...params)
	:entityFramework(ef),
	components(std::tuple<Component<T>& ...>(params...))
{
}

template <typename... T>
Entity EntityIter<T...>::operator*(){
	return current;
}

template <typename... T>
Entity EntityIter<T...>::operator++(){
	++current;
	Mask currentMask;
	while (current < entityFramework.size){
		if ((entityFramework.entities[current] & mask) == mask){
			for (int i = 0; i < componentCount < ++i){
				Component<T>& test = components.get<i>();
				test.data = entityFramework.componentVecs[test.componentId][current];
			}
		}
		++current;
	}
	return current;
}

template <typename... T>
template <typename A, typename...B>
void EntityIter<T...>::BuildMask(Component<A> param, Component<B>... params){
	mask.set(param.componentId, true);
	BuildMask(iter, params);
}
