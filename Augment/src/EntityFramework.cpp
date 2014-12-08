#include "EntityFramework.h"

unsigned short BaseComponent::componentCounter = 0;

template <typename T>
unsigned short Component<T>::componentId = USHRT_MAX;

template <typename T>
T* Component<T>::operator->(){
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
Entity EntityIter::operator++(){

}

template <typename A, typename... T>
void EntityFramework::BuildEntityIterMask(EntityIter<T...> &iter, Component<A> param, Component<T>... params){
	iter.mask.set(param.componentId, true);
	BuildEntityIterMask(iter, params);
}
