#ifndef _Median_h
#define _Median_h

#include <assert.h>
#include <memory>

#undef min
#undef max
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
class Median
{
public:
	Median() : m_size() {}
	virtual ~Median() {}

	virtual void	Clear() = 0 
	{
		m_size = 0;
	}
	virtual void	Insert(const T& value) = 0
	{
		++m_size;
	}

	virtual bool	GetMedian(T& median) const = 0;

protected:
	int				m_size;
};

#endif // _Median_h