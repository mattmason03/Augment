#pragma once

#include <vector>
#include <array>
#include <tuple>
#include <bitset>

#include "Pool.h"

#define MAX_COMPONENTS 64

template<size_t Size>
class EntityFramework;
using Entity = unsigned int;

using Mask = std::bitset < MAX_COMPONENTS > ;

struct BaseComponent{
public:
	static unsigned short componentCounter;
};

template <typename TData>
struct Component : BaseComponent{
public:
	static unsigned short componentId;
	using Type = TData;

	TData& operator->();

	Component<TData>();

	static void Register();
};

template <typename... TComp>
class EntityIter{
public:

	template<size_t TEntityCt>
	EntityIter(Entity cur, EntityFramework<TEntityCt>& ef, TComp* ...params);

	Entity current;

	std::tuple<TComp*...> components;
	std::tuple_size<std::tuple<TComp*...>> componentCount;

	Mask mask;

	template <typename... TCompA>
	bool operator!=(const EntityIter<TCompA...>& x) const;
	Entity operator*();
	Entity operator++();

	template <typename TCompA, typename... TCompB>
	void BuildMask(TCompA param, TCompB...params);

	EntityFramework& entityFramework;
};

template <size_t TEntityCt>
class EntityFramework{
public:

	EntityFramework();

	Entity CreateEntity();
	
	template <typename TComp>
	void RegisterComponent();

	template <typename TComp>
	void AddComponent(Entity entity);

	template <typename TComp>
	TComp* GetComponent(Entity entity);

	template <typename TComp>
	void RemoveComponent(Entity entity);

	template <typename... TComp>
	EntityIter<TComp...> begin(TComp&... params);

	template <typename... TComp>
	EntityIter<TComp...> end();

	unsigned int count;

private:

	std::array<Mask, TEntityCt> entities;
	std::array<Pool*, MAX_COMPONENTS> compPools;

	template <typename... TComp>
	struct iteratorFriends{
		friend class EntityIter<TComp...>;
	};

	
};





/////
//IMPLEMENTATION
/////


unsigned short BaseComponent::componentCounter = 0;

template <typename TData>
unsigned short Component<TData>::componentId = USHRT_MAX;

template <typename TData>
TData& Component<TData>::operator->(){
	return data;
}

template <typename TData>
void Component<TData>::Register(){
	if (componentId == USHRT_MAX)
		componentId = componentCounter++;
}

template <typename... TComp>
template <size_t Size>
EntityIter<TComp...>::EntityIter(Entity cur, EntityFramework<Size>& ef, TComp* ...params)
	:current(cur),
	entityFramework(ef),
	components(std::tuple<TComp* ...>(params...))
{
	BuildMask(params...);
}

template <typename... TComp>
template <typename... A>
bool EntityIter<TComp...>::operator!=(const EntityIter<A...>& x) const{
	return current != x.current;
}

template <typename... TComp>
Entity EntityIter<TComp...>::operator*(){
	return current;
}

template <typename... TComp>
Entity EntityIter<TComp...>::operator++(){
	++current;
	Mask currentMask;
	while (current < entityFramework.count){
		if ((entityFramework.entities[current] & mask) == mask){
			for (int i = 0; i < componentCount; ++i){
				TComp& test = components.get<i>();
				test = entityFramework.compPools[test.componentId]->Get(current);
			}
		}
		++current;
	}
	return current;
}

template <typename... TComp>
template <typename A, typename...B>
void EntityIter<TComp...>::BuildMask(A param, B... params){
	mask.set(param.componentId, true);
	BuildMask(iter, params);
}

template <size_t TEntityCt>
EntityFramework<TEntityCt>::EntityFramework()
	:count(0)
{
}

template <size_t TEntityCt>
Entity EntityFramework<TEntityCt>::CreateEntity(){
	return count++;
}

template <size_t TEntityCt>
template <typename TComp>
void EntityFramework<TEntityCt>::RegisterComponent(){
	TComp::Register();
	compPools[TComp::componentId] = new CompPool <TEntityCt, TComp >;
}

template <size_t TEntityCt>
template <typename TComp>
void EntityFramework<TEntityCt>::AddComponent(Entity entity){
	entities[entity].set(TComp::componentId, true);
}

template <size_t TEntityCt>
template <typename TComp>
TComp* EntityFramework<TEntityCt>::GetComponent(Entity entity){
	return (TComp*)compPools[TComp::componentId]->Get(entity);
}

template <size_t TEntityCt>
template <typename TComp>
void EntityFramework<TEntityCt>::RemoveComponent(Entity entity){
	entities[entity].reset(TComp::componentId);
}

template <size_t TEntityCt>
template <typename... TComp>
EntityIter<TComp...> EntityFramework<TEntityCt>::begin(TComp&... params)
{
	EntityIter<TEntityCt, TComp...> iter(this, params...);
	return iter;
}

template <size_t TEntityCt>
template <typename... TComp>
EntityIter<TComp...> EntityFramework<TEntityCt>::end()
{
	EntityIter<TEntityCt> iter(count, this);
	return iter;
}
