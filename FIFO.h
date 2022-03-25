/******************************************************************************
Copyright (c) 2022 SoC Design Laboratory, Konkuk University, South Korea
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met: redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer;
redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution;
neither the name of the copyright holders nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Authors: Jooho Wang (joohowang@konkuk.ac.kr)

Revision History
2022.03.25: Started by Jooho Wang
*******************************************************************************/

#ifndef FIFO_H

#define FIFO_H


template<class T>
class FIFO {

public: 

	FIFO():_depth(2){};

	void setting(int depth);

private:

	// parameter

	int _depth;

	// action variables
	bool push, pop;

	int head_w, tail_w;
	bool notFull_w, notEmpty_w;

	T in_data0;
	T out_data0;

	// update variables
	T* data0;

	int head, tail;
	bool notFull, notEmpty;

	// selective update index
	int write_idx;

public:

	void reset();

	void action(
		T wdata,
		bool wvalid,
		bool rready
		)
	{	


		if (wvalid)
			in_data0 = wdata;

		push = wvalid && notFull;
		pop  = rready && notEmpty;

		if (push)
		{			
			write_idx = tail;

			if (tail == _depth-1)
				tail_w = 0;
			else
			{
				tail_w = tail +1;
			}

		}
		else
			write_idx = -1;


		if (pop)
		{
			if (head == _depth-1)
				head_w = 0;
			else
			{
				head_w = head +1;
			}

		}

		if (tail_w == head_w)
		{
			if (push)
			{
				notFull_w  = 0;
				notEmpty_w = 1;
			}
			else if (pop)
			{
				notFull_w  = 1;
				notEmpty_w = 0;
			}
		}
		else
		{
			notFull_w = 1;
			notEmpty_w= 1;
		}

	}

	void update(
		bool& wready,
		T& rdata,

		bool& rvalid
		)
	{
		// var update

		head = head_w;
		tail = tail_w;
		notFull = notFull_w;
		notEmpty= notEmpty_w;

		if (write_idx>=0)	// selective update
		{
			data0[write_idx] = in_data0;
		}

		// output update 

		wready = notFull_w;

		rdata = data0[head];

		rvalid = notEmpty_w;

	}

	void update(
		bool&	wready,
		T&		rdata,

		bool&	rvalid,

		int&	num_elements
		)
	{
		update(wready,rdata,rvalid);

		if (head_w > tail_w || !notFull)
			num_elements = _depth + tail_w - head_w;
		else
			num_elements = tail_w - head_w;
	}

};

template <class T>
void
	FIFO<T>::setting(int depth)
{
	if (depth<2)
		_depth = 2;
	else
		_depth = depth;
}

template <class T>
void
	FIFO<T>::reset(){

		data0 = new T[_depth];

		for (int ii=0; ii<_depth; ii++)
		{
			data0[ii] = 0;
		}

		head_w = 0;
		tail_w = 0;
		notFull_w = 1;
		notEmpty_w = 0;
		in_data0 =0;
		out_data0 =0;
		write_idx = -1;
}

#endif