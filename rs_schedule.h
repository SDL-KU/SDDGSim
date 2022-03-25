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

void local_link_update(PE1 new_PE[R][E], int cycle)
{
	for (int ee = 0; ee < E; ee++)
	{
		for (int rr = 1; rr < R; rr++)
		{
			if (cycle < E)
			{
				for (int pp = 0; pp < p; pp++)
				{
					new_PE[rr][ee].spad_ofmap[pp] = new_PE[rr - 1][ee].spad_ofmap[pp];
				}
			}
			else
			{
				if (new_PE[rr][ee].ofmap_gon_handshake_curr)
				{
					new_PE[rr][ee].spad_ofmap[new_PE[rr][ee].o_t_ptr] = new_PE[rr - 1][ee].spad_ofmap[new_PE[rr][ee].o_t_ptr];
				}
			}
		}
	}
}

void check_handshake(PE1 new_PE[R][E], bool *input_hs, bool *weight_hs, bool *output_hs, int i_index, int w_index, int o_index)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if ((rr + ee) == i_index % H)
			{
				*input_hs &= new_PE[rr][ee].ifmap_handshake_curr;
			}

			if (rr == w_index % R)
			{
				*weight_hs &= new_PE[rr][ee].fmap_handshake_curr;
			}

			if (rr == 0 && ee == o_index % E)
			{
				*output_hs &= new_PE[rr][ee].ofmap_gin_handshake_curr;
			}
		}
	}
}

void update_output_str_node(nodeInfo *node, int str_output_id, int cycle, bool *OK)
{
	int curr_cycle = cycle;

	if (str_output_id > 0)
	{
		for (int pid = 0; pid < 10; pid++)
		{
			if (node[str_output_id].parentid[pid] < 0)
				continue;

			int parent_id = node[str_output_id].parentid[pid];

			if (!strcmp(node[parent_id].microop, "Add"))
			{
				if (node[parent_id].complete_cycle == curr_cycle)
				{
					node[str_output_id].start_cycle = node[parent_id].start_cycle + 1;
					node[str_output_id].complete_cycle = node[parent_id].complete_cycle + 1;
					node[str_output_id].used = TRUE;
					node[str_output_id].parent_dependence[0] = parent_id;
					*OK = TRUE;
				}
			}
		}
	}
	else
	{
		*OK = FALSE;
	}
}

void make_valid_set(PE1 new_PE[R][E], int *input_valid_set, int *weight_valid_set, int *output_valid_set, int i_index, int w_index, int o_index)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (i_index < H*W*C) {
				if ((rr + ee) == i_index % H)
					input_valid_set[E*rr + ee] = 1;
				else
					input_valid_set[E*rr + ee] = 0;
			}
			else
				input_valid_set[E*rr + ee] = 0;

			if (w_index < R*S*C*M)
			{
				if (rr == w_index % R)
					weight_valid_set[E*rr + ee] = 1;
				else
					weight_valid_set[E*rr + ee] = 0;
			}
			else
				weight_valid_set[E*rr + ee] = 0;

			if (o_index < E*F*M)
			{
				if (rr == 0 && ee == o_index % E)
					output_valid_set[E*rr + ee] = 1;
				else
					output_valid_set[E*rr + ee] = 0;
			}
			else
				output_valid_set[E*rr + ee] = 0;
		}
	}
}

void mac_update(nodeInfo *node, PE1 new_PE[R][E], int cycle)
{
	bool input_exist = FALSE;
	bool weight_exist = FALSE;
	bool output_exist = FALSE;

	int curr_input_id = 0, curr_weight_id = 0, curr_output_id = 0;
	int curr_input_pixel = 0, curr_weight_pixel = 0, curr_output_pixel = 0;

	int i_t_index = 0;
	int w_t_index = 0;
	int o_t_index = 0;

	//
	int tmp = 0;

	for (int id = 0; id < NODE_NUM; id++)
	{
		if (*node[id].microop == NULL || node[id].used == TRUE)
			continue;

		input_exist = FALSE;
		weight_exist = FALSE;
		output_exist = FALSE;

		if (!strcmp(node[id].microop, "Mul"))
		{
			for (int rr = 0; rr < R; rr++)
			{
				for (int ee = 0; ee < E; ee++)
				{
					i_t_index = new_PE[rr][ee].i_t_ptr;
					curr_input_id = new_PE[rr][ee].spad_ifmap[i_t_index];

					if (curr_input_id < 0)
						continue;

					curr_input_pixel = node[curr_input_id].pixel;

					w_t_index = new_PE[rr][ee].f_t_ptr;
					curr_weight_id = new_PE[rr][ee].spad_fmap[w_t_index];

					if (curr_weight_id < 0)
						continue;

					curr_weight_pixel = node[curr_weight_id].pixel;

					o_t_index = new_PE[rr][ee].o_t_ptr;
					curr_output_id = new_PE[rr][ee].spad_ofmap[o_t_index];

					if (curr_output_id < 0)
						continue;

					curr_output_pixel = node[curr_output_id].pixel;

					for (int pid = 0; pid < 10; pid++)
					{
						if (node[id].parentid[pid] < 0)
							continue;

						int parent_id = node[id].parentid[pid];
						int parent_pixel = node[parent_id].pixel;

						if (!strcmp(node[parent_id].label, "ofmap"))
						{
							if (parent_pixel == curr_output_pixel)
							{
								output_exist = TRUE;
							}
							else
							{
								output_exist = FALSE;
							}
						}
						else if (!strcmp(node[parent_id].label, "ifmap"))
						{
							if (parent_pixel == curr_input_pixel)
							{
								input_exist = TRUE;
							}
							else
							{
								input_exist = FALSE;
							}
						}
						else if (!strcmp(node[parent_id].label, "fmap"))
						{
							if (parent_pixel == curr_weight_pixel)
							{
								weight_exist = TRUE;
							}
							else
							{
								weight_exist = FALSE;
							}
						}


						if (input_exist == TRUE && weight_exist == TRUE && output_exist == TRUE && new_PE[rr][ee].mult_enable_curr)
						{
							node[id].start_cycle = cycle;
							node[id].complete_cycle = cycle;

							//Update PE Information
							node[id].pe_info = 2 * rr + 1 * ee;

							for (int cid = 0; cid < 10; cid++)
							{
								if (node[id].childid[cid] < 0)
									continue;

								int child_id = node[id].childid[cid];

								if (!strcmp(node[child_id].microop, "Add"))
								{
									node[child_id].start_cycle = node[id].complete_cycle;
									node[child_id].complete_cycle = node[id].complete_cycle;

									node[child_id].pe_info = 2 * rr + 1 * ee;
									node[child_id].used = TRUE;
								}
							}
							node[id].used = TRUE;
							weight_exist = FALSE;
							weight_exist = FALSE;
							weight_exist = FALSE;
						}
					}
				}
			}
		}
	}
}

void rs_dataflow_scheduling(nodeInfo *node, PE1 new_PE[R][E], input_memory *i_mem, weight_memory *w_mem, output_memory *o_mem, output_memory *o_mem_str)
{
	int cycle = 0;
	int tmp = 0;
	int i_index = 0; int w_index = 0; int o_index = 0; int o_str_index = 0;

	int *input_valid_set = (int *)calloc(H*W*C, sizeof(int));
	int *weight_valid_set = (int *)calloc(R*S*C*M, sizeof(int));
	int *output_valid_set = (int *)calloc(E*F*M, sizeof(int));

	int output_id = 0;
	int input_id = 0;
	int weight_id = 0;
	int str_output_id = 0;

	bool STR_OK = FALSE;
	bool LDR_OK = FALSE;

	bool input_order_complete = FALSE;
	bool weight_order_complete = FALSE;
	bool output_order_complete = FALSE;
	bool output_str_order_complete = FALSE;

	bool input_hs = TRUE;
	bool weight_hs = TRUE;
	bool output_hs = TRUE;
	bool output_index_update = TRUE;

	//initialize
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			pe_member_var_initializer(new_PE, rr, ee);
		}
	}

	// Start RS Scheduling
	while (1)
	{
		cout << "CYCLE : " << cycle << endl;
		
		//Initialze
		input_hs = TRUE;
		weight_hs = TRUE;
		output_hs = TRUE;
		STR_OK = FALSE;
		LDR_OK = FALSE;
		//Initialize
		
		input_id      = find_load_node_input(node, i_index, input_name, i_mem, cycle, input_order_complete);
		weight_id     = find_load_node_weight(node, w_index, weight_name, w_mem, cycle, weight_order_complete);
		output_id     = find_load_node_output(node, o_index, output_name, o_mem, cycle, output_order_complete);
		str_output_id = find_store_node_output(node, o_str_index, output_name, o_mem_str, output_str_order_complete);
		make_valid_set(new_PE, input_valid_set, weight_valid_set, output_valid_set, i_index, w_index, o_index);

		/////////////////////////////////////////////
		pe_main_action(new_PE);
		if (cycle == 0)
		{
			pe_main_update(new_PE, input_id, weight_id, output_id, input_valid_set, weight_valid_set, output_valid_set);
			for (int rr = 0; rr<R; rr++)
				for (int ee = 0; ee<E; ee++)
					fifo_to_spad(new_PE, rr, ee, input_id, weight_id, output_id);

			local_link_update(new_PE,cycle);

			for (int rr = 0; rr < R; rr++)
				for (int ee = 0; ee < E; ee++) {
					cout << rr << "   " << ee << "  " << endl;
					for (int pp = 0; pp < p; pp++)
						cout << new_PE[rr][ee].spad_ofmap[pp] << endl;
				}

			cin >> tmp;
		}
		else
		{
			pe_main_action(new_PE);
			pe_main_update(new_PE, input_id, weight_id, output_id , input_valid_set, weight_valid_set, output_valid_set);
			mac_update(node, new_PE, cycle);
			update_output_str_node(node, str_output_id, cycle, &STR_OK);

			for (int rr = 0; rr<R; rr++)
				for (int ee = 0; ee<E; ee++)
					fifo_to_spad(new_PE, rr, ee, input_id, weight_id, output_id);

			local_link_update(new_PE, cycle);

			for (int rr = 0; rr < R; rr++)
				for (int ee = 0; ee < E; ee++) {
					cout << rr << "   " << ee << "  " << endl;
					for (int pp = 0; pp < p; pp++)
						cout << new_PE[rr][ee].spad_ofmap[pp] << endl;
				}

			cin >> tmp;
		}
		/////////////////////////////////////////////

		//for (int rr = 0; rr < R; rr++)
		//{
		//	for (int ee = 0; ee < E; ee++)
		//	{
		//		cout << rr << "   " << ee << endl;
		//		for (int qs = 0; qs < q*S; qs++)
		//		{
		//			cout << new_PE[rr][ee].spad_ifmap[qs] << endl;
		//			cout << new_PE[rr][ee].ifmap_handshake_cnt << endl;
		//		}
		//	}
		//}
		//cin >> tmp;

		//for (int rr = 0; rr < R; rr++)
		//	for (int ee = 0; ee < E; ee++)
		//		cout << "KKK: " << new_PE[rr][ee].ofmap_gon_handshake_curr << endl;
		//cin >> tmp;
		//for (int rr = 0; rr < R; rr++)
		//	for (int ee = 0; ee < E; ee++)
		//		cout << "QQQ: " << new_PE[rr][ee].ofmap_gin_handshake_curr << endl;
		//cin >> tmp;

		/////////////////////////////////////////////
		check_handshake(new_PE, &input_hs, &weight_hs, &output_hs, i_index, w_index, o_index);
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		if (input_hs) 
			i_index++;

		cout << i_index << endl;
		
		if (i_index == H * W * C * (M / p))
		{
			input_order_complete = TRUE;
			i_index = i_index;
		}

		if (weight_hs)
			w_index++;

		if (w_index == R * S * C * M)
		{
			weight_order_complete = TRUE;
			w_index = w_index;
		}

		if (output_hs)
		{
			o_index++;
		}
		
		if (o_index == E * F * M)
		{
			output_order_complete = TRUE;
			o_index = o_index;
		}

		if (STR_OK == TRUE)
		{
			if (o_str_index < E * F * M)
			{
				o_str_index++;
			}

			if (o_str_index == E * F * M)
			{
				o_str_index = 0;
				output_str_order_complete = TRUE;
			}
		}
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		cycle++;
		if (cycle >= MAX_CYCLE)
		{
			break;
		}
		/////////////////////////////////////////////
	}
}