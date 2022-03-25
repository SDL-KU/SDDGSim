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

void mult_update(nodeInfo *node, input_spad i_spad[R][E], weight_spad w_spad[R][E], output_spad o_spad[R][E], int *input_loaded, int *weight_loaded, int *output_loaded, int cycle)
{
	//int input_pixel = 0, input_id = 0, input_loaded_pixel[H*W*C] = { -999, };
	//int weight_pixel = 0, weight_id = 0, weight_loaded_pixel[R*S*C*M] = { -999, };
	//int output_pixel = 0, output_id = 0, output_loaded_pixel[R*S*C*M] = { -999, };
	//int tmp = 0;

	int curr_input_id = 0, curr_weight_id = 0, curr_output_id = 0;
	int curr_input_pixel = 0, curr_weight_pixel = 0, curr_output_pixel = 0;
	bool input_exist = FALSE;
	bool weight_exist = FALSE;
	bool output_exist = FALSE;

	//
	int input_t_index = 0;
	int weight_t_index = 0;
	int output_t_index = 0;

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
					input_t_index = i_spad[rr][ee].t_index;
					curr_input_pixel = node[i_spad[rr][ee].reg[input_t_index]].pixel;
					curr_input_id = i_spad[rr][ee].reg[input_t_index];

					weight_t_index = w_spad[rr][ee].t_index; 
					curr_weight_pixel = node[w_spad[rr][ee].reg[weight_t_index]].pixel;
					curr_weight_id = w_spad[rr][ee].reg[weight_t_index];

					for (int pid = 0; pid < 10; pid++)
					{
						if (node[id].parentid[pid] < 0)
							continue;

						int parent_id = node[id].parentid[pid];
						int parent_pixel = node[parent_id].pixel;

						if (!strcmp(node[parent_id].label, "ofmap"))
						{
							for (int pp = 0; pp < p; pp++)
							{
								for (int rr1 = 0; rr1 < R; rr1++)
								{
									for (int ee1 = 0; ee1 < E; ee1++)
									{
										if (parent_pixel == node[o_spad[rr1][ee1].reg[pp]].pixel)
										{
											output_exist = TRUE;
											curr_output_id = o_spad[rr1][ee1].reg[pp];
										}
									}
								}
							}
						}
						else if (!strcmp(node[parent_id].label, "ifmap"))
						{
							if (curr_input_pixel == parent_pixel)
								input_exist = TRUE;
							else
								input_exist = FALSE;
						}
						else if (!strcmp(node[parent_id].label, "fmap"))
						{
							if (curr_weight_pixel == parent_pixel)
								weight_exist = TRUE;
							else
								weight_exist = FALSE;
						}

						if (input_exist == TRUE && weight_exist == TRUE && output_exist == TRUE)
						{
							node[id].start_cycle = cycle;
							node[id].complete_cycle = cycle;

							// Update PE Number
							node[id].pe_info = 2 * rr + 1 * ee;

							// Update Parent
							for (int kk = 0; kk < 3; kk++)
							{
								if (kk == 0)
								{
									node[id].parent_dependence[kk] = curr_input_id;
								}
								else if (kk == 1)
								{
									node[id].parent_dependence[kk] = curr_weight_id;
								}
								else
								{
									node[id].parent_dependence[kk] = curr_output_id;
								}
							}

							for (int ccid = 0; ccid < 10; ccid++)
							{
								if (node[id].childid[ccid] < 0)
									continue;

								int child_id = node[id].childid[ccid];

								if (!strcmp(node[child_id].microop, "Add"))
								{
									node[child_id].start_cycle = node[id].start_cycle;
									node[child_id].complete_cycle = node[id].complete_cycle;
									
									for (int kk = 0; kk < 2; kk++)
									{
										if (kk == 0)
										{
											node[child_id].parent_dependence[kk] = curr_output_id;
										}
										else
										{
											node[child_id].parent_dependence[kk] = id;
										}
									}

									node[child_id].pe_info = 2 * rr + 1 * ee;
									node[child_id].used = TRUE;
								}
							}
							node[id].used = TRUE;
							input_exist = FALSE;
							weight_exist = FALSE;
							output_exist = FALSE;
						}
					}
				}
			}
		}
	}
}

void update_mac_info(nodeInfo *node, int r_index, int e_index, int cycle)
{
	int curr_cycle = cycle;
	int id = -1;
	int tmp = 0;

	// SET MULT NODE 
	while (id != NODE_NUM - 1)
	{
		id++;

		if (*node[id].microop == NULL)
			continue;

		if (!strcmp(node[id].microop, "Mul"))
		{
			if (node[id].complete_cycle == curr_cycle)
			{
				if (node[id].pe_info_set == FALSE)
				{
					node[id].pe_info = 2 * r_index + 1 * e_index;
					node[id].pe_info_set = TRUE;
					break;
				}
			}
		}
	}

	id = -1;

	//SET ADD NODE
	while (id != NODE_NUM - 1)
	{
		id++;

		if (*node[id].microop == NULL)
			continue;

		if (!strcmp(node[id].microop, "Add"))
		{
			if (node[id].complete_cycle == curr_cycle)
			{
				if (node[id].pe_info_set == FALSE)
				{
					node[id].pe_info = 2 * r_index + 1 * e_index;
					node[id].pe_info_set = TRUE;
					break;
				}
			}
		}
	}
}

void pe_update(nodeInfo *node, pe PE[R][E], input_spad i_spad[R][E], weight_spad w_spad[R][E], output_spad o_spad[R][E], int cycle)
{
	//int tmp;
	bool input_pe_pos[R][E] = { 0, };
	bool weight_pe_pos[R][E] = { 0, };
	bool output_pe_pos[R][E] = { 0, };
	int id = -1;
	int tmp = 0;

	for (int kk = 0; kk < R; kk++)
	{
		for (int uu = 0; uu < E; uu++)
		{
			PE[kk][uu].prev_mac_count = PE[kk][uu].mac_count;
		}
	}

	while(id <= NODE_NUM-1)
	{
		id++;
		if (*node[id].microop == NULL)
			continue;

		// INITIALIZE
		for (int rr = 0; rr < R; rr++)
		{
			for (int ee = 0; ee < E; ee++)
			{
				input_pe_pos[rr][ee] = FALSE;
				weight_pe_pos[rr][ee] = FALSE;
				output_pe_pos[rr][ee] = FALSE;
			}
		}

		if (!strcmp(node[id].microop, "Mul"))
		{
			if (node[id].complete_cycle != cycle)
				continue;
			
			//
			for (int p_index = 0; p_index < 10; p_index++)
			{
				if (node[id].parentid[p_index] == INITIALIZE)
					continue;

				int p_id1 = node[id].parentid[p_index];

				if (!strcmp(node[p_id1].label, "ifmap"))
				{
					for (int rr = 0; rr < R; rr++)
					{
						for (int ee = 0; ee < E; ee++)
						{
							for (int qs = 0; qs < q*S; qs++)
							{
								if (node[i_spad[rr][ee].reg[qs]].pixel == node[p_id1].pixel)
								{
									input_pe_pos[rr][ee] |= TRUE;
								}
								else
								{
									input_pe_pos[rr][ee] |= FALSE;
								}
							}
						}
					}
				}

			}
			
			for (int p_index = 0; p_index < 10; p_index++)
			{
				if (node[id].parentid[p_index] == INITIALIZE)
					continue;

				int p_id2 = node[id].parentid[p_index];

				if (!strcmp(node[p_id2].label, "fmap"))
				{
					for (int rr = 0; rr < R; rr++)
					{
						for (int ee = 0; ee < E; ee++)
						{
							for (int pqs = 0; pqs < p*q*S; pqs++)
							{
								if (node[w_spad[rr][ee].reg[pqs]].pixel == node[p_id2].pixel)
								{
									weight_pe_pos[rr][ee] |= TRUE;
								}
								else
								{
									weight_pe_pos[rr][ee] |= FALSE;
								}
							}
						}
					}
				}
			}

			for (int p_index = 0; p_index < 10; p_index++)
			{
				if (node[id].parentid[p_index] == INITIALIZE)
					continue;

				int p_id3 = node[id].parentid[p_index];

				if (!strcmp(node[p_id3].label, "ofmap"))
				{
					for (int rr = 0; rr < R; rr++)
					{
						for (int ee = 0; ee < E; ee++)
						{
							for (int pp = 0; pp < p; pp++)
							{
								if (node[o_spad[rr][ee].reg[pp]].pixel == node[p_id3].pixel)
								{
									output_pe_pos[rr][ee] |= TRUE;
								}
								else
								{
									output_pe_pos[rr][ee] |= FALSE;
								}
							}
						}
					}
				}
			}

			// update mac count
			for (int rr = 0; rr < R; rr++)
			{
				for (int ee = 0; ee < E; ee++)
				{
					bool PE_POS = 0;
					PE_POS = input_pe_pos[rr][ee] && weight_pe_pos[rr][ee] && output_pe_pos[rr][ee];

					if (PE_POS == TRUE)
					{
						PE[rr][ee].mac_count++;

						//for (int kk = 0; kk < R; kk++)
						//{
						//	for (int uu = 0; uu < E; uu++)
						//	{
						//		cout << PE[kk][uu].prev_mac_count << "   " << PE[kk][uu].mac_count << endl;;
						//	}
						//}

						//cout <<"MAC COUNT: "<<rr <<"   " << ee<< "  "<< PE[rr][ee].mac_count << endl;

						//update_mac_info(node, rr, ee, cycle);

						PE[rr][ee].pe_used = TRUE;

						if (PE[rr][ee].mac_count == q*S)
						{
							PE[rr][ee].mac_count = 0;
						}
					}
				}
			}
		} // MUL 
	} // NODE


	//for (int rr = 0; rr < R; rr++)
	//	for (int ee = 0; ee<E; ee++)
	//		cout << "MAC COUNT: " << rr << "   " << ee << "  " << PE[rr][ee].prev_mac_count << "    " << PE[rr][ee].mac_count << endl;
			
}

//void str_node_update(nodeInfo *node, int str_output_id, int cycle, bool *OK )
//{
//	int curr_cycle = cycle;
//
//	for (int pid = 0; pid < 10; pid++)
//	{
//		if (node[str_output_id].parentid[pid] < 0)
//			continue;
//
//		int parent_id = node[str_output_id].parentid[pid];
//
//		if (!strcmp(node[parent_id].microop, "Add"))
//		{
//			if (node[parent_id].complete_cycle == curr_cycle)
//			{
//				node[str_output_id].start_cycle = node[parent_id].start_cycle + 1;
//				node[str_output_id].complete_cycle = node[parent_id].complete_cycle + 1;
//				node[str_output_id].used = TRUE;
//				node[str_output_id].parent_dependence[0] = parent_id;
//				*OK = TRUE;
//			}
//		}
//	}
//}

