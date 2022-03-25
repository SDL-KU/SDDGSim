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

void output_spad_update(nodeInfo *node, pe PE[R][E], output_spad o_spad[R][E], int output_id, int ucast_out_pixel, int select_col, int cycle, bool *OK, bool output_order_complete)
{
	// Update Local Link 

}

void weight_spad_update(nodeInfo *node, weight_spad w_spad[R][E], pe PE[R][E], int weight_id, int e_index, int r_index, int curr_cycle, bool weight_order_complete)
{
	int tmp = 0;
	int select_row = curr_cycle % R;
	int curr_index = w_spad[select_row][e_index].r_index;

	if (weight_order_complete != TRUE)
	{
		// update node info
		node[weight_id].start_cycle = curr_cycle;
		node[weight_id].complete_cycle = curr_cycle;

		// update spad info
		for (int ee = 0; ee < E; ee++)
		{
			w_spad[select_row][ee].reg[curr_index] = weight_id;
			w_spad[select_row][ee].r_index++;

			for (int uu = 0; uu < R; uu++)
			{
				if (PE[uu][ee].mac_count != PE[uu][ee].prev_mac_count)
				{
					w_spad[uu][ee].t_index++;

					if (w_spad[uu][ee].t_index >= p * q * S)
						w_spad[uu][ee].t_index = 0;
				}
			}
		}
	}
	else
	{
		for (int ee = 0; ee < E; ee++)
		{
			for (int uu = 0; uu < R; uu++)
			{
				if (PE[uu][ee].mac_count != PE[uu][ee].prev_mac_count)
				{
					w_spad[uu][ee].t_index++;

					if (w_spad[uu][ee].t_index >= p * q * S)
						w_spad[uu][ee].t_index = 0;
				}
			}
		}
	}
}

void input_spad_update(nodeInfo *node, input_spad i_spad[R][E], pe PE[R][E] ,int input_id, int e_index, int r_index, int curr_cycle, bool input_order_complete)
{
	int tmp = 0;
	int curr_index = i_spad[r_index][e_index].r_index;

	if (input_order_complete != TRUE)
	{
		if (i_spad[r_index][e_index].full_flag == FALSE)
		{
			if (PE[r_index][e_index].mac_count != PE[r_index][e_index].prev_mac_count)
			{
				i_spad[r_index][e_index].t_index++;
				if (i_spad[r_index][e_index].t_index >= q * S)
				{
					int t_idx_cnt = i_spad[r_index][e_index].t_index_cnt % S;
				
					i_spad[r_index][e_index].t_index = q * S - ((t_idx_cnt + 1) * q);
					i_spad[r_index][e_index].t_index_cnt++;
				}
			}

			if (curr_cycle % (R + E - 1) == 0)
			{
				if (r_index + e_index == 0)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}
			else if (curr_cycle % (R + E - 1) == 1)
			{
				if (r_index + e_index == 1)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}
			else if (curr_cycle % (R + E - 1) == 2)
			{
				if (r_index + e_index == 2)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}
		}
		else
		{
			if (curr_cycle % (R + E - 1) == 0)
			{
				if (r_index + e_index == 0)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}
			else if (curr_cycle % (R + E - 1) == 1)
			{
				if (r_index + e_index == 1)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}
			else if (curr_cycle % (R + E - 1) == 2)
			{
				if (r_index + e_index == 2)
				{
					// update node info.
					node[input_id].start_cycle = curr_cycle;
					node[input_id].complete_cycle = curr_cycle;
					i_spad[r_index][e_index].reg[curr_index] = input_id;
					i_spad[r_index][e_index].r_index++;
					// reuse!
					if (i_spad[r_index][e_index].r_index >= q * S)
					{
						i_spad[r_index][e_index].full_flag = TRUE;
						i_spad[r_index][e_index].r_index = 0;
					}
				}
			}

			if (PE[r_index][e_index].mac_count != PE[r_index][e_index].prev_mac_count)
			{
				// update spad info
				i_spad[r_index][e_index].t_index++;
				if (i_spad[r_index][e_index].t_index >= q * S)
				{
					int t_idx_cnt = i_spad[r_index][e_index].t_index_cnt % S;

					i_spad[r_index][e_index].t_index = q * S - ((t_idx_cnt + 1) * q);
					i_spad[r_index][e_index].t_index_cnt++;
				}
			}
		}
	}
}