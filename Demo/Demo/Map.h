#ifndef  _Map_h_
#define _Map_h_

#include <map>

#include "Median.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Map of values and number of their occurances
 */
template <class T, class Compare = std::less<T>>
class Map
	: public Median<T, Compare>
{
	using BaseClass = Median<T, Compare>;

public:
	Map();
	
	virtual void	Clear();
	virtual void	Insert(const T& value);

	virtual bool	GetMedian(T& median) const;

private:
	std::map<T, int, Compare>	m_values;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
Map<T, Compare>::Map()
	: m_values()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ void Map<T, Compare>::Clear()
{
	BaseClass::Clear();
	m_values.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ void Map<T, Compare>::Insert(const T& value)
{
	BaseClass::Insert(value);
	++m_values[value];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ bool Map<T, Compare>::GetMedian(T& median) const
{
	if (!BaseClass::m_size)
		return false;

	int	steps = BaseClass::m_size / 2;
	for (const auto& val : m_values)
	{
		if (steps >= val.second)
		{
			steps -= val.second;
			continue;
		}
		
		if(BaseClass::m_size % 2)
		{
			median = val.first;
		}
		else if (steps)
		{
			median = val.first;
		}
		else
		{
			auto	prev = m_values.find(val.first);
			assert(prev != m_values.end());
			assert(prev != m_values.begin());
			--prev;
			median = (val.first + prev->first) / 2;
		}

		break;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _Map_h_