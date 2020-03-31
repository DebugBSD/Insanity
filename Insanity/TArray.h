#pragma once

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

template<typename T>
class TArray
{
private:
	T* m_pElem;
	size_t m_Size;
	size_t m_CurrentIndex;
	size_t m_Growth;

public:
	TArray(size_t size = 1, size_t growth = 1):
		m_pElem{new T[size]},
		m_Size{size},
		m_CurrentIndex{0},
		m_Growth{growth}
	{
		
	}

	T& operator[](size_t i)
	{
		return m_pElem[i];
	}

	void Append(const int index, const T& data)
	{
		if (index < m_CurrentIndex - 1)
		{
			m_pElem[index] = data;
		}
		else
		{
			const size_t lastSize = m_CurrentIndex;
			T* newData = new T[index + 1];
			std::copy(m_pElem, m_pElem + lastSize, newData);
			delete[] m_pElem;
			m_pElem = newData;
			m_pElem[index] = data;
			m_CurrentIndex = index + 1;
		}
	}
};

