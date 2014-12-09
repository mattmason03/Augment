#pragma once

#include <vector>
#include <tuple>
#include <bitset>

#define MAX_COMPONENTS 64

class EntityFramework;
using Entity = unsigned int;
using Mask = std::bitset < MAX_COMPONENTS > ;

struct BaseComponent{
public:
	static unsigned short componentCounter;
};

template <typename DataType>
struct Component : BaseComponent{
public:
	static unsigned short componentId;

	DataType& data;
	DataType& operator->();

	Component<DataType>();

	void Register();
};

template <typename... T>
class EntityIter{
public:

	EntityIter(EntityFramework ef, Component<T>& ...params);

	Entity current;

	std::tuple<Component<T>&...> components;
	std::tuple_size<std::tuple<Component<T>&...>> componentCount;

	Mask mask;

	bool operator!=(const EntityIter& x) const;
	Entity operator*();
	Entity operator++();

	template <typename A, typename... B>
	void BuildMask(Component<A> param, Component<B>...params);

	EntityFramework& entityFramework;
	friend class EntityFramework;
};

class EntityFramework{
public:

	EntityFramework(unsigned int maxEntities);

	Entity CreateEntity();
	
	template <typename T>
	void AddComponent(Entity entity);

	template <typename T>
	T* GetComponent(Entity entity);

	template <typename T>
	void RemoveComponent(Entity entity);

	template <typename... T>
	EntityIter<T...> begin(T&... params);

	template <typename... T>
	EntityIter<T...> end();

	unsigned int size;

private:

	template <typename A, typename... T>
	void BuildEntityIterMask(EntityIter<T...> &iter, Component<A> param, Component<T>... params);

	std::vector<Mask> entities;

	std::vector<char> indirectComponentPool;

	std::vector<std::vector<BaseComponent>*> componentVecs;

	template <typename... T>
	struct iteratorFriends{
		friend class EntityIter<T...>;
	};
};