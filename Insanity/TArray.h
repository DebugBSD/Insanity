/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2020, DebugBSD
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

