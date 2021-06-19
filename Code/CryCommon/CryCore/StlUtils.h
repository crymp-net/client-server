//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//	STL Utils header
//	
//	File: STLutils.h
//	Description : Various convenience utility functions for STL and alike
//  Used in Animation subsystem, and in some tools
//
//	History:
//	-:Created by Sergiy Migdalskiy
//
//////////////////////////////////////////////////////////////////////
#ifndef _STL_UTILS_HEADER_
#define _STL_UTILS_HEADER_

#include <algorithm>
#include <map>
#include <vector>

// auto-cleaner: upon destruction, calls the clear() method
template <class T>
class CAutoClear
{
public:
	CAutoClear (T* p): m_p(p) {}
	~CAutoClear () {m_p->clear();}
protected:
	T* m_p;
};


template <class Container>
unsigned sizeofArray (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.size());
}

template <class Container>
unsigned sizeofVector (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.capacity());
}

template <class Container>
unsigned sizeofArray (const Container& arr, unsigned nSize)
{
	return arr.empty()?0u:(unsigned)(sizeof(typename Container::value_type)*nSize);
}

template <class Container>
unsigned capacityofArray (const Container& arr)
{
	return (unsigned)(arr.capacity()*sizeof(arr[0]));
}

template <class T>
unsigned countElements (const std::vector<T>& arrT, const T& x)
{
	unsigned nSum = 0;
	for (typename std::vector<T>::const_iterator iter = arrT.begin(); iter != arrT.end(); ++iter)
		if (x == *iter)
			++nSum;
	return nSum;
}

// [Timur]
/** Contain extensions for STL library.
*/
namespace stl
{
	//////////////////////////////////////////////////////////////////////////
	//! Searches the given entry in the map by key, and if there is none, returns the default value
	//////////////////////////////////////////////////////////////////////////
	template <typename Map>
	inline typename Map::mapped_type find_in_map(const Map& mapKeyToValue, const typename Map::key_type& key, typename Map::mapped_type valueDefault)
	{
		typename Map::const_iterator it = mapKeyToValue.find (key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	// searches the given entry in the map by key, and if there is none, returns the default value
	// The values are taken/returned in REFERENCEs rather than values
	template <typename Map>
	inline typename Map::mapped_type& find_in_map_ref(Map& mapKeyToValue, const typename Map::key_type& key, typename Map::mapped_type& valueDefault)
	{
		typename Map::iterator it = mapKeyToValue.find (key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of map.
	//////////////////////////////////////////////////////////////////////////
	template <class Map,class Vector>
	inline void map_to_vector( const Map& theMap,Vector &array )
	{
		array.resize(0);
		array.reserve( theMap.size() );
		for (typename Map::const_iterator it = theMap.begin(); it != theMap.end(); ++it)
		{
			array.push_back( it->second );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of set.
	//////////////////////////////////////////////////////////////////////////
	template <class Set,class Vector>
	inline void set_to_vector( const Set& theSet,Vector &array )
	{
		array.resize(0);
		array.reserve( theSet.size() );
		for (typename Set::const_iterator it = theSet.begin(); it != theSet.end(); ++it)
		{
			array.push_back( *it );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find and erase element from container.
	// @return true if item was find and erased, false if item not found.
	//////////////////////////////////////////////////////////////////////////
	template <class Container,class Value>
		inline bool find_and_erase( Container& container,const Value &value )
	{
		typename Container::iterator it = std::find( container.begin(),container.end(),value );
		if (it != container.end())
		{
			container.erase( it );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container,class Value>
		inline bool push_back_unique( Container& container,const Value &value )
	{
		if (std::find(container.begin(),container.end(),value) == container.end())
		{
			container.push_back( value );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container,class Iter>
	inline void push_back_range( Container& container,Iter begin,Iter end )
	{
		for (Iter it = begin; it != end; ++it)
		{
			container.push_back(*it);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in container.
	// @return true if item found.
	template <class Container,class Value>
		inline bool find( Container& container,const Value &value )
	{
		return std::find(container.begin(),container.end(),value) != container.end();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	//
	template <class Iterator,class T>
		inline Iterator binary_find(Iterator first,Iterator last,const T& value)
	{
		Iterator it = std::lower_bound(first,last,value);
		return (it == last || value != *it) ? last : it;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// @return true if item was inserted.
	template <class Container,class Value>
		inline bool binary_insert_unique( Container& container,const Value &value )
	{
		typename Container::iterator it = std::lower_bound(container.begin(),container.end(),value);
		if (it != container.end())
		{
			if (*it == value)
				return false;
			container.insert( it,value );
		}
		else
			container.insert( container.end(),value );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// and erases if element found.
	// @return true if item was erased.
	template <class Container,class Value>
		inline bool binary_erase( Container& container,const Value &value )
	{
		typename Container::iterator it = std::lower_bound(container.begin(),container.end(),value);
		if (it != container.end() && *it == value)
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	struct container_object_deleter
	{
		template<typename T>
			void operator()(const T* ptr) const
		{
			delete ptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Convert arbitary class to const char*
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
		inline const char* constchar_cast( const Type &type )
	{
		return type;
	}

	//! Specialization of string to const char cast.
	template <>
		inline const char* constchar_cast( const string &type )
	{
		return type.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Case sensetive less key for any type convertable to const char*.
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
	struct less_strcmp
	{
		bool operator()( const Type &left,const Type &right ) const
		{
			return strcmp(constchar_cast(left),constchar_cast(right)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case insensetive less key for any type convertable to const char*.
	template <class Type>
	struct less_stricmp
	{
		bool operator()( const Type &left,const Type &right ) const
		{
			return stricmp(constchar_cast(left),constchar_cast(right)) < 0;
		}
	};
}

#endif
