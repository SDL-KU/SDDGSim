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

void input_mem_order(input_memory *i_mem)
{
	int tmp = 0;
	int n_ptr = 0; int q_ptr = 0; int row_ptr = 0; int col_ptr = 0;
	int mem_size = N*H*W*C;

	for (int ii = 0; ii<mem_size; ii++)
	{
		tmp = H * W*C*n_ptr + H * W*q_ptr + W * row_ptr + col_ptr;
		i_mem->order[ii] = tmp * 1;

		if (row_ptr < H - 1)
			row_ptr++;
		else {
			row_ptr = 0;
			if (q_ptr<C - 1)
				q_ptr++;
			else {
				q_ptr = 0;
				if (col_ptr<W - 1)
					col_ptr++;
				else {
					col_ptr = 0;
					if (n_ptr<N - 1)
						n_ptr++;
					else {
						n_ptr = 0;
					}
				}
			}
		}
	}
}

void weight_mem_order(weight_memory *w_mem)
{
	int tmp = 0;
	int p_ptr = 0; int q_ptr = 0; int row_ptr = 0; int col_ptr = 0;
	int mem_size = R*S*M*C;

	for (int ii = 0; ii<mem_size; ii++)
	{
		tmp = R * S*C*p_ptr + R * S*q_ptr + S * row_ptr + col_ptr;
		w_mem->order[ii] = tmp * 1;

		if (row_ptr < R - 1)
			row_ptr++;
		else {
			row_ptr = 0;
			if (p_ptr<M - 1)
				p_ptr++;
			else {
				p_ptr = 0;
				if (q_ptr<C - 1)
					q_ptr++;
				else {
					q_ptr = 0;
					if (col_ptr<S - 1)
						col_ptr++;
					else {
						col_ptr = 0;
					}
				}
			}
		}

	}
}

void output_mem_order(output_memory *o_ldr_mem, output_memory *o_str_mem)
{
	int tmp = 0;
	int tmp1 = 0;
	int n_ptr = 0; int q_ptr = 0; int row_ptr = 0; int col_ptr = 0;
	int mem_size = E*F*M*N;

	for (int ii = 0; ii < mem_size; ii++)
	{
		tmp = E*F*M*n_ptr + E * F*q_ptr + F * row_ptr + col_ptr;

		o_ldr_mem->order[ii] = tmp * 1;
		o_str_mem->order[ii] = tmp * 1;

		if (row_ptr < E - 1)
			row_ptr++;
		else {
			row_ptr = 0;
			if (q_ptr<M - 1)
				q_ptr++;
			else {
				q_ptr = 0;
				if (col_ptr<F - 1)
					col_ptr++;
				else {
					col_ptr = 0;
					if (n_ptr<N - 1)
						n_ptr++;
					else {
						n_ptr = 0;
					}
				}
			}
		}
	}
}

void initialze_mem_order(input_memory *i_mem, weight_memory *w_mem, output_memory *o_ldr_mem, output_memory *o_str_mem)
{
	// input memory pixel order
	input_mem_order(i_mem);

	// weight memory pixel order
	weight_mem_order(w_mem);

	// output memory pixel order
	output_mem_order(o_ldr_mem, o_str_mem);
}

//////////////////////////////////////////////////////////////


//////////////////Initialize PE//////////////////////////
void initialize_PE(PE aladdin_pe[R][E])
{

	/////////////////SET FIFO///////////////////////////
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee<E; ee++)
		{
			aladdin_pe[rr][ee].setting(100, 100, 100);
		}
	}
	///////////////////////////////////////////////////

	////////Member variable initialze/////////
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			aladdin_pe[rr][ee].pe_member_var_initializer();
		}
	}
	///////////////////////////////////////////////////
}

