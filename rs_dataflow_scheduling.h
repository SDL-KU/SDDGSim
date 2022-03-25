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

void make_valid_set(bool (*glb_to_fifo_input_vld)[E], bool (*glb_to_fifo_weight_vld)[E], bool (*glb_to_fifo_output_vld)[E], int i_index, int w_index, int o_index)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (i_index < H*W*C) {
				if ((rr + ee) == i_index % H)
					glb_to_fifo_input_vld[rr][ee] = 1;
				else
					glb_to_fifo_input_vld[rr][ee] = 0;
			}
			else
				glb_to_fifo_input_vld[rr][ee] = 0;

			if (w_index < R*S*C*M)
			{
				if (rr == w_index % R)
					glb_to_fifo_weight_vld[rr][ee] = 1;
				else
					glb_to_fifo_weight_vld[rr][ee] = 0;
			}
			else
				glb_to_fifo_weight_vld[rr][ee] = 0;

			if (o_index < E*F*M)
			{
				if (rr == 0 && ee == o_index % E)
					glb_to_fifo_output_vld[rr][ee] = 1;
				else
					glb_to_fifo_output_vld[rr][ee] = 0;
			}
			else
				glb_to_fifo_output_vld[rr][ee] = 0;
		}
	}
}

void map_node_id(int(*glb_to_pe_fifo_input)[E], int(*glb_to_pe_fifo_weight)[E], int(*glb_to_pe_fifo_output)[E], int input_id, int weight_id, int output_id)
{
	// map to input
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			glb_to_pe_fifo_input[rr][ee] = input_id;
			glb_to_pe_fifo_weight[rr][ee] = weight_id;
			glb_to_pe_fifo_output[rr][ee] = output_id;
		}
	}
}

void local_link_update(PE aladdin_PE[R][E], int cycle)
{
	int tmp;

	for (int ee = 0; ee < E; ee++)
	{
		for (int rr = 1; rr < R; rr++)
		{
			if (cycle < E*p)
			{
				for (int pp = 0; pp < p; pp++)
				{
					aladdin_PE[rr][ee].spad_ofmap[pp] = aladdin_PE[rr - 1][ee].spad_ofmap[pp];
				}
			}
			else
			{

				if (aladdin_PE[rr][ee].ofmap_gon_handshake_curr)//if (aladdin_PE[rr][ee].ofmap_gin_handshake_past)
				{
					//cout << "PE: " << E * rr + ee << endl;
					//cout << "CYCLE: " << cycle << "  " << aladdin_PE[rr][ee].ofmap_gon_handshake_curr << endl;
					int r_ptr = 0;
					r_ptr = aladdin_PE[rr][ee].o_r_ptr;

					//for (int pp = 0; pp < p; pp++)
					//	cout << "pe_spad_of[" << pp << "] = " << aladdin_PE[rr][ee].spad_ofmap[pp] << endl;

					aladdin_PE[rr][ee].spad_ofmap[r_ptr] = aladdin_PE[rr - 1][ee].spad_ofmap[r_ptr];

					//cout << "o_r_ptr : " << r_ptr << endl;

					//for (int pp = 0; pp < p; pp++)
					//	cout << "pe_spad_of[" << pp << "] = " << aladdin_PE[rr][ee].spad_ofmap[pp] << endl;

					//cin >> tmp;
					/*if (cycle == 25)
					{
						cout << "CYCLE: " << cycle << endl;
						cout << "KASDASDKASD" << aladdin_PE[rr][ee].ofmap_gon_handshake_curr << endl;

						cout << "curr_cycle: " << cycle << endl;
						for (int rr = 0; rr < R; rr++) {
							for (int ee = 0; ee < E; ee++) {
								cout << "PE: " << E * rr + ee << endl;
								for (int pp = 0; pp < p; pp++)
								{
									cout << aladdin_PE[rr][ee].spad_ofmap[pp] << endl;
								}
							}
						}

						cin >> tmp;
					}*/
				}

				/*
				if (aladdin_PE[rr][ee].ofmap_gin_handshake_curr)
				{
					int t_ptr = 0;
					if (aladdin_PE[rr][ee].ofmap_gin_handshake_curr)
					{
						t_ptr = aladdin_PE[rr][ee].o_r_ptr;
					}
					else
					{
						t_ptr = aladdin_PE[rr][ee].o_t_ptr;
					}
			
					aladdin_PE[rr][ee].spad_ofmap[t_ptr] = aladdin_PE[rr - 1][ee].spad_ofmap[t_ptr];

					if (cycle == 25)
					{
						cout << "CYCLE: " << cycle << endl;
						cout << "KASDASDKASD" << aladdin_PE[rr][ee].ofmap_gon_handshake_curr << endl;

						cout << "curr_cycle: " << cycle << endl;
						for (int rr = 0; rr < R; rr++) {
							for (int ee = 0; ee < E; ee++) {
								cout << "PE: " << E * rr + ee << endl;
								for (int pp = 0; pp < p; pp++)
								{
									cout << aladdin_PE[rr][ee].spad_ofmap[pp] << endl;
								}
							}
						}
					
						cin >> tmp;
					}
				}
				*/
			}
		}
	}
}

void mac_update(nodeInfo *node, PE aladdin_PE[R][E], int cycle)
{
	bool input_exist = FALSE;
	bool weight_exist = FALSE;
	bool output_exist = FALSE;

	int curr_input_id = 0, curr_weight_id = 0, curr_output_id = 0;
	int curr_input_pixel = 0, curr_weight_pixel = 0, curr_output_pixel = 0;

	int i_t_index = 0;
	int w_t_index = 0;
	int o_t_index = 0;

	int parent_iid = 0;
	int parent_wid = 0;
	int parent_ipix = 0;
	int parent_wpix = 0;

	//
	int tmp = 0;
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			//////////////////////////////////////////////////////////
			i_t_index = aladdin_PE[rr][ee].i_t_ptr;
			curr_input_id = aladdin_PE[rr][ee].spad_ifmap[i_t_index];

			if (curr_input_id < 0)
				continue;

			curr_input_pixel = node[curr_input_id].pixel;
			//////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////
			w_t_index = aladdin_PE[rr][ee].f_t_ptr;
			curr_weight_id = aladdin_PE[rr][ee].spad_fmap[w_t_index];

			if (curr_weight_id < 0)
				continue;

			curr_weight_pixel = node[curr_weight_id].pixel;
			//////////////////////////////////////////////////////////

			for (int id = 0; id < NODE_NUM; id++)
			{

				if (*node[id].microop == NULL || node[id].used == TRUE)
					continue;

				input_exist = FALSE;
				weight_exist = FALSE;

				if (!strcmp(node[id].microop, "Mul"))
				{
					for (int pid = 0; pid < NOP; pid++)
					{
						if (node[id].parentid[pid] < 0)
							continue;

						int parent_id = node[id].parentid[pid];
						int parent_pixel = node[parent_id].pixel;
						
						if (!strcmp(node[parent_id].label, "ifmap"))
						{
							if (parent_pixel == curr_input_pixel)
							{
								parent_ipix = parent_pixel;
								parent_iid = parent_id;
								input_exist = TRUE;
							}
							else
							{
								input_exist = FALSE;
								parent_ipix = 0;
							}
						}
					}

					for (int pid = 0; pid < NOP; pid++)
					{
						if (node[id].parentid[pid] < 0)
							continue;

						int parent_id = node[id].parentid[pid];
						int parent_pixel = node[parent_id].pixel;
						parent_wpix = parent_pixel;

						if (!strcmp(node[parent_id].label, "fmap"))
						{
							if (parent_pixel == curr_weight_pixel)
							{
								parent_wpix = parent_pixel;
								parent_wid = parent_id;
								weight_exist = TRUE;
							}
							else
							{
								weight_exist = FALSE;
								parent_wpix = 0;
							}
						}
					}

					if (input_exist == TRUE && weight_exist == TRUE && aladdin_PE[rr][ee].mult_enable_curr)
					{
						node[id].start_cycle = cycle;
						node[id].complete_cycle = cycle;

						//Update PE Information
						node[id].pe_info = E * rr +  ee;

						for (int cid = 0; cid < NOP; cid++)
						{
							if (node[id].childid[cid] < 0)
								continue;

							int child_id = node[id].childid[cid];

							if (!strcmp(node[child_id].microop, "Add"))
							{
								node[child_id].start_cycle = node[id].complete_cycle;
								node[child_id].complete_cycle = node[id].complete_cycle;

								node[id].pe_info = E * rr + ee;
								node[child_id].used = TRUE;
							}
						}
						node[id].used = TRUE;
						input_exist = FALSE;
						weight_exist = FALSE;
					}
				}
			}
		}
	}
}

void check_handshake(nodeInfo *node, PE aladdin_PE[R][E], bool *input_hs, bool *weight_hs, bool *output_hs, int i_index, int w_index, int o_index, bool (*glb_to_fifo_input_vld)[E], bool(*glb_to_fifo_weight_vld)[E], bool(*glb_to_fifo_output_vld)[E], bool (*aladdin_pe_input_wrdy)[E], bool (*aladdin_pe_weight_wrdy)[E], bool(*aladdin_pe_output_wrdy)[E], int input_id, int weight_id, int output_id, int curr_cycle)
{
	int tmp = 0;
	int node_id = 0;

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if ((rr + ee) == i_index % H)
			{
				*input_hs &= glb_to_fifo_input_vld[rr][ee] &  aladdin_pe_input_wrdy[rr][ee];
			}

			if (rr == w_index % R)
			{
				*weight_hs &= glb_to_fifo_weight_vld[rr][ee] & aladdin_pe_weight_wrdy[rr][ee];
			}

			if (rr == 0 && ee == o_index % E)
			{
				*output_hs &= glb_to_fifo_output_vld[rr][ee] & aladdin_pe_output_wrdy[rr][ee];
			}
		}
	}

	// Batch Pixel
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (aladdin_PE[rr][ee].ifmap_handshake_curr == TRUE)
			{
				node_id = aladdin_PE[rr][ee].ifmap_data_from_fifo;
				node[node_id].start_cycle = curr_cycle;
				node[node_id].complete_cycle = curr_cycle;
			}

			if (aladdin_PE[rr][ee].fmap_handshake_curr == TRUE)
			{
				node_id = aladdin_PE[rr][ee].fmap_data_from_fifo;
				node[node_id].start_cycle = curr_cycle;
				node[node_id].complete_cycle = curr_cycle;
			}
			
			if (rr == 0)
			{
				if (aladdin_PE[rr][ee].ofmap_gin_handshake_curr == TRUE)
				{
					node_id = aladdin_PE[rr][ee].ofmap_data_from_fifo;
					node[node_id].start_cycle = curr_cycle;
					node[node_id].complete_cycle = curr_cycle;
				}
			}
		}
	}

}

void update_output_str_nodes(nodeInfo *node,PE aladdin_pe[R][E], int curr_cycle)
{
	int str_id_cnt = 0;
	int tmp = 0;

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			str_id_cnt = 0;

			if (rr == R - 1)
			{
				if (aladdin_pe[rr][ee].ofmap_gon_valid_curr)
				{
					//cout <<"curr_cycle: "<< curr_cycle << endl;
					

					int t_ptr = 0;
					if (aladdin_pe[rr][ee].ofmap_gin_handshake_curr)
					{
						t_ptr = aladdin_pe[rr][ee].o_r_ptr;
					}
					else
					{
						t_ptr = aladdin_pe[rr][ee].o_t_ptr;
					}

					int node_id = aladdin_pe[rr][ee].spad_ofmap[t_ptr];
					int str_pixel = node[node_id].pixel;

					//cout << "node_id: " << node_id << endl;
					//cout << "str_pixel: " << str_pixel << endl;
					
						
					//if (curr_cycle >= 22)
					//{
					//	cout << rr << "   " << ee << endl;
					//	cout <<"CURR CYCLES: "<< curr_cycle << endl;
					//	cout << "t_ptr: " << t_ptr << endl;
					//	cout << "aladdin_pe[rr][ee].ofmap_gon_valid_curr: " << aladdin_pe[rr][ee].ofmap_gon_valid_curr << endl;
					//	cout << "node[node_id].pixel: " << node[node_id].pixel << endl;
					//
					//	for (int pp = 0; pp < p; pp++)
					//	{
					//		cout << "ID: "<< aladdin_pe[rr][ee].spad_ofmap[pp] << endl;
					//	}
					//
					//	cin >> tmp;
					//}

					for (int id = 0; id < NODE_NUM; id++)
					{
						if (*node[id].microop == NULL)
							continue;

						if (!strcmp(node[id].microop, "Store"))
						{
							if (node[id].pixel == str_pixel)
							{
								//str_id_cnt++;
								//if (str_id_cnt == q * S * R)
								//{
								//	node[id].start_cycle = curr_cycle + 1;
								//	node[id].complete_cycle = curr_cycle + 1;
								//	node[id].used = TRUE;
								//}

								//
								node[id].start_cycle = curr_cycle + 1;
								node[id].complete_cycle = curr_cycle + 1;
								node[id].used = TRUE;
								//
							}
						}
					}
				}
			}
		}
	}
}

void add_local_link(nodeInfo *node, PE aladdin_pe[R][E], int curr_cycle ,int *loc_add_index)
{
	int index = *loc_add_index;
	int tmp = 0;
	//int NUM_NODE = NODE_NUM + NODE_NUM_LOC;

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (rr > 0)
			{
				if (aladdin_pe[rr][ee].ofmap_gin_handshake_curr)
				{
					//cout << "curr_cycle: " << curr_cycle << endl;

					int t_ptr = 0;
					if (aladdin_pe[rr][ee].ofmap_gin_handshake_curr)
					{
						t_ptr = aladdin_pe[rr][ee].o_r_ptr;
					}
					else
					{
						t_ptr = aladdin_pe[rr][ee].o_t_ptr;
					}

					int node_id = aladdin_pe[rr][ee].spad_ofmap[t_ptr];
					int pixel = node[node_id].pixel;

					node[NODE_NUM + index].start_cycle = curr_cycle;
					node[NODE_NUM + index].complete_cycle = curr_cycle;
					node[NODE_NUM + index].pe_info = E * rr + ee;
					node[NODE_NUM + index].pixel = pixel;

					index++;
				}
			}
			else
			{
				continue;
			}
		}
	}

	*loc_add_index = index;
}

void rs_dataflow_w_fifo(nodeInfo *node, PE aladdin_pe[R][E], input_memory *i_mem, weight_memory *w_mem, output_memory *o_ldr_mem, output_memory *o_str_mem, int last_node_id)
{
	int curr_cycle = 0;

	// node variable
	int i_index = 0;
	int w_index = 0;
	int o_ldr_index = 0;
	int o_str_index = 0;
	int input_id = 0;
	int weight_id = 0;
	int output_id = 0;
	int str_output_id = 0;

	bool input_order_complete  = FALSE;
	bool weight_order_complete = FALSE;
	bool output_order_complete = FALSE;
	bool output_str_order_complete = FALSE;

	//Init. ready signal of PE
	bool aladdin_pe_input_wrdy[R][E];
	bool aladdin_pe_weight_wrdy[R][E];
	bool aladdin_pe_output_gin_wrdy[R][E];

	// localink
	bool aladdin_pe_local_link_rdy[R - 1][E];
	bool aladdin_pe_local_link_vld[R - 1][E];
	bool aladdin_pe_to_x_mc_gon_vld[R][E];
	int loc_add_index = 0;

	// psum & local link value
	int aladdin_pe_psum[R][E];
	int aladdin_local_link_val[R][E];

	int glb_to_pe_fifo_input[R][E];
	int glb_to_pe_fifo_weight[R][E];
	int glb_to_pe_fifo_output[R][E];

	bool glb_to_fifo_input_vld[R][E];
	bool glb_to_fifo_weight_vld[R][E];
	bool glb_to_fifo_output_vld[R][E];

	// handshake check
	bool input_hs = TRUE;
	bool weight_hs = TRUE;
	bool output_hs = TRUE;

	bool STR_OK = FALSE;
	bool LDR_OK = FALSE;

	//initialize variable
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee<E; ee++)
		{ 
			aladdin_pe[rr][ee].reset();
	
			if (rr < R - 1)
			{
				aladdin_pe_local_link_vld[rr][ee] = FALSE;
			}
		}
	}

	//Init. aladdin PE update
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (rr == 0) {
				aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
										  aladdin_pe_output_gin_wrdy[rr][ee], aladdin_local_link_val[rr][ee],
										  aladdin_pe_local_link_vld[rr][ee], rr, ee, curr_cycle);
			}
			else if (rr == R - 1) {
				aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
										  aladdin_pe_local_link_rdy[rr - 1][ee], aladdin_pe_psum[rr][ee],
										  aladdin_pe_to_x_mc_gon_vld[rr][ee],
										  rr, ee, curr_cycle);
			}
			else {
				aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
										  aladdin_pe_local_link_rdy[rr - 1][ee], aladdin_local_link_val[rr][ee],
										  aladdin_pe_local_link_vld[rr][ee],
										  rr, ee, curr_cycle);
			}
		}
	}

	int tmp;

	// START 
	while (1)
	{
		curr_cycle++;
		cout << "cycle : " << curr_cycle << endl;

		//////////////////////////////////////////////
		input_hs = TRUE;
		weight_hs = TRUE;
		output_hs = TRUE;
		STR_OK = FALSE;
		LDR_OK = FALSE;
		//////////////////////////////////////////////

		input_id = find_load_node_input(node, i_index, input_name, i_mem, curr_cycle, input_order_complete);
		weight_id = find_load_node_weight(node, w_index, weight_name, w_mem, curr_cycle, weight_order_complete);
		output_id = find_load_node_output(node, o_ldr_index, output_name, o_ldr_mem, curr_cycle, output_order_complete);
		make_valid_set(glb_to_fifo_input_vld, glb_to_fifo_weight_vld, glb_to_fifo_output_vld, i_index, w_index, o_ldr_index);

		map_node_id(glb_to_pe_fifo_input, glb_to_pe_fifo_weight, glb_to_pe_fifo_output, input_id, weight_id, output_id);

		//PE Action				
		for (int rr = 0; rr<R; rr++)
		{
			for (int ee = 0; ee<E; ee++)
			{
				if (rr == 0) {
					aladdin_pe[rr][ee].action(glb_to_pe_fifo_input[rr][ee],
											  glb_to_pe_fifo_weight[rr][ee],
											  glb_to_pe_fifo_output[rr][ee],
											  glb_to_fifo_input_vld[rr][ee] ,
											  glb_to_fifo_weight_vld[rr][ee] ,
											  glb_to_fifo_output_vld[rr][ee] ,
											  aladdin_pe_local_link_rdy[rr][ee],
											  rr);
				}
				else if (rr == R - 1) {
					aladdin_pe[rr][ee].action(glb_to_pe_fifo_input[rr][ee],
											  glb_to_pe_fifo_weight[rr][ee],
					    		    		  aladdin_local_link_val[rr - 1][ee],
											  glb_to_fifo_input_vld[rr][ee] ,
											  glb_to_fifo_weight_vld[rr][ee] ,
											  aladdin_pe_local_link_vld[rr - 1][ee],TRUE, 
											  rr);
				}
				else {
					aladdin_pe[rr][ee].action(glb_to_pe_fifo_input[rr][ee],
											  glb_to_pe_fifo_weight[rr][ee],
											  aladdin_local_link_val[rr - 1][ee],
											  glb_to_fifo_input_vld[rr][ee] ,
											  glb_to_fifo_weight_vld[rr][ee] ,
											  aladdin_pe_local_link_vld[rr - 1][ee] ,
											  aladdin_pe_local_link_rdy[rr][ee],
											  rr);
				}
			}
		}


		// PE Update
		for (int rr = 0; rr < R; rr++)
		{
			for (int ee = 0; ee < E; ee++)
			{
				if (rr == 0) {
					aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
											  aladdin_pe_output_gin_wrdy[rr][ee], aladdin_local_link_val[rr][ee],
											  aladdin_pe_local_link_vld[rr][ee], rr, ee, curr_cycle);
				}
				else if (rr == R - 1) {
					aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
											  aladdin_pe_local_link_rdy[rr - 1][ee], aladdin_pe_psum[rr][ee],
											  aladdin_pe_to_x_mc_gon_vld[rr][ee], rr, ee, curr_cycle);
				}
				else {
					aladdin_pe[rr][ee].update(aladdin_pe_input_wrdy[rr][ee], aladdin_pe_weight_wrdy[rr][ee],
											  aladdin_pe_local_link_rdy[rr - 1][ee], aladdin_local_link_val[rr][ee],
											  aladdin_pe_local_link_vld[rr][ee], rr, ee, curr_cycle);
				}
			}
		}

		// Update MAC nodes
		mac_update(node, aladdin_pe, curr_cycle);

		// Update Spads
		for (int rr = 0; rr < R; rr++)
		{
			for (int ee = 0; ee < E; ee++)
			{
				aladdin_pe[rr][ee].fifo_to_spad(rr, aladdin_local_link_val[rr][ee]);
			}
		}

		// Update Store nodes
		update_output_str_nodes(node, aladdin_pe, curr_cycle);

		// Update Local link add nodes
		add_local_link(node, aladdin_pe, curr_cycle, &loc_add_index);

		//cout << "pe[1][0].local_value : " << aladdin_local_link_val[1][0]<<endl;
		//cout << "pe[1][0].local_valid : " << aladdin_pe_local_link_vld[1][0] << endl;
		//cout << "pe[1][0].local_ready : " << aladdin_pe_local_link_rdy[1][0] << endl;
		//cout << "pe[2][0].fifo_of_value : " << aladdin_pe[2][0].ofmap_data_from_fifo << endl;
		//cout << "pe[2][0].fifo_of_valid : " << aladdin_pe[2][0].ofmap_valid_gin << endl;
		//cout << "pe[2][0].fifo_of_ready : " << aladdin_pe[2][0].ofmap_gin_ready_curr << endl;
		//cout << "pe[2][0].hands curr : " << aladdin_pe[2][0].ofmap_gin_handshake_curr << endl;
		//cout << "pe[2][0].v curr : " << aladdin_pe[2][0].ofmap_gon_valid_curr << endl;
		//for (int rr = 0; rr < R; rr++)
		//{
		//	for (int ee = 0; ee < E; ee++)
		//	{
		//		cout << "PE: " << E * rr + ee << endl;
		//		for (int pp = 0; pp < p; pp++)
		//			cout << aladdin_pe[rr][ee].spad_ofmap[pp] << endl;
		//	}
		//}
		//cin >> tmp;

		// Update Input, Weight, Output pixel nodes
		check_handshake(node, aladdin_pe, &input_hs, &weight_hs, &output_hs, i_index, w_index, o_ldr_index, glb_to_fifo_input_vld, glb_to_fifo_weight_vld, glb_to_fifo_output_vld,aladdin_pe_input_wrdy, aladdin_pe_weight_wrdy, aladdin_pe_output_gin_wrdy, input_id, weight_id, output_id, curr_cycle);

		/////////////////////////////////////////////
		if (input_hs)
			i_index++;

		if (i_index == H * W * C)
		{
			input_order_complete = TRUE;
			i_index = i_index;
		}
		////////////////////////////////////////////

		////////////////////////////////////////////
		if (weight_hs)
			w_index++;

		if (w_index == R * S * C * M)
		{
			weight_order_complete = TRUE;
			w_index = w_index;
		}
		////////////////////////////////////////////

		////////////////////////////////////////////
		if (output_hs)
		{
			o_ldr_index++;
		}

		if (o_ldr_index == E * F * M)
		{
			output_order_complete = TRUE;
			o_ldr_index = o_ldr_index;
		}
		////////////////////////////////////////////

		//
		if ((node[last_node_id].used  == TRUE) && (curr_cycle > node[last_node_id].complete_cycle))
		{
			break;
		}
		//

		////////////////////////////////////////////
	}
}
