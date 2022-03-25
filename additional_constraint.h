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

#define INPUT_FIFO_SIZE (H*W*C)
#define WEIGHT_FIFO_SIZE (R*S*C*M)
#define OUTPUT_FIFO_SIZE (E*F*M)

void initialize_node_status(nodeInfo *node)
{
	for (int id = 0; id < NODE_NUM + NODE_NUM_LOC; id++)
	{
		if (*node[id].microop == NULL)
			continue;

		node[id].used = FALSE;
		node[id].start_cycle = -1;
		node[id].complete_cycle = -1;
	}
}

void initialize_fifo_mem(int **fifo_name, int total_pixel_num, int bank_num)
{
	for (int i = 0; i < bank_num; i++)
	{
		fifo_name[i] = (int *)calloc((total_pixel_num/bank_num) + 1, sizeof(int));
	}
}

void free_fifo_mem(int **bank, int bank_num)
{
	for (int pp = 0; pp < bank_num; pp++)
	{
		free(bank[pp]);
	}
	free(bank);
}

void initialize_output_load_flag(bool output_load_flag[E][p])
{
	for (int ee = 0; ee < E; ee++)
	{
		for (int pp = 0; pp < p; pp++)
		{
			output_load_flag[ee][pp] = TRUE;
		}
	}
}

void update_input_pixel_count(int input_pixel_order[R][E][W*q], int input_pixel_count[R][E][W*q], input_memory *i_mem)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			for (int order_index = 0; order_index < W * q; order_index++)
			{
				input_pixel_order[rr][ee][order_index] = i_mem->order[H * order_index + rr + ee];

				int temp_input_index = (input_pixel_order[rr][ee][order_index] % (H*W)) % H + 1;

				if (temp_input_index < S)
				{
					input_pixel_count[rr][ee][order_index] = p * (temp_input_index % S);
				}
				else if ((W + 1 - temp_input_index) < S)
				{
					input_pixel_count[rr][ee][order_index] = p * ((W + 1 - temp_input_index) % S);
				}
				else
				{
					input_pixel_count[rr][ee][order_index] = p * S;
				}
			}
		}
	}
}

void update_weight_pixel_order(int weight_pixel_order[R][p * q * S], weight_memory *w_mem)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int order_index = 0; order_index < p * q * S; order_index++)
		{
			weight_pixel_order[rr][order_index] = w_mem->order[R * order_index + rr];
		}
	}
}

void update_output_pixel_order(int output_pixel_order[E][F*p], output_memory *o_ldr_mem)
{
	for (int ee = 0; ee < E; ee++)
	{
		for (int order_index = 0; order_index < F * p; order_index++)
		{
			output_pixel_order[ee][order_index] = o_ldr_mem->order[E * order_index + ee];
		}
	}
}


void initialize_mac_order_index(int mac_order_index[R][E])
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			mac_order_index[rr][ee] = 1;
		}
	}
}

void update_i_pixel_to_bank(int *i_bank_number, int input_pixel_num, int bank_num, input_memory *i_mem)
{
	for (int ii = 0; ii < input_pixel_num; ii++)
	{
		int pixel = i_mem->order[ii];
		i_bank_number[pixel] = ii % bank_num;
	}
}

void update_w_pixel_to_bank(int *w_bank_number, int weight_pixel_num, int bank_num, weight_memory *w_mem)
{
	for (int ww = 0; ww < weight_pixel_num; ww++)
	{
		int pixel = w_mem->order[ww];
		w_bank_number[pixel] = ww % bank_num;
	}
}

void update_o_pixel_to_bank(int *o_bank_number, int output_pixel_num, int bank_num, output_memory *o_ldr_mem)
{
	for (int oo = 0; oo < output_pixel_num; oo++)
	{
		int pixel = o_ldr_mem->order[oo];
		o_bank_number[pixel] = oo % bank_num;
	}
}

void initialize_mac(bool mac_check[R][E])
{
	for (int rr = 0; rr < R; rr++)// PE mac check initialize
	{
		for (int ee = 0; ee < E; ee++)
		{
			mac_check[rr][ee] = FALSE;
		}
	}
}

void update_constraint_mac(nodeInfo *node, int pe_mac_order_id[R][E][p * q * S * F + 1], int pe_mac_cnt[R][E][p], int input_reg[R][E][q * S], int input_pixel_order[R][E][W*q], int input_pixel_count[R][E][W*q], int output_reg[R][E][p], bool mac_check[R][E], int local_link_mul_id[R][E][p], int loc_add[R][E][p], int mac_order_index[R][E], int output_store_id[E][p], bool *is_output_loading, int output_loc_index[E*F*M], bool output_load_flag[E][p], int curr_cycle)
{
	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++) //find PE
		{
			//PE cannot set MAC or loc add while loading output at first row of PE
			if ((rr == 0) && (is_output_loading[ee]))
			{
				mac_check[rr][ee] = TRUE;
			}

			//LOC ADD
			for (int loc_check = 0; loc_check < p; loc_check++) //check loc add signal
			{
				if (loc_add[rr][ee][loc_check] > 0) //if local link add signal is found
				{
					int loc_pixel = node[loc_add[rr][ee][loc_check]].pixel;//loc add output pixel
					if (output_loc_index[loc_pixel] == (rr - 1)) //loc add order check
					{
						for (int loc_id = NODE_NUM; loc_id < NODE_NUM + NODE_NUM_LOC; loc_id++) //find loc add node to be set 
						{
							if ((node[loc_id].used == FALSE) && (node[loc_add[rr][ee][loc_check]].pixel == node[loc_id].pixel))
							{
								node[loc_id].start_cycle = curr_cycle;
								node[loc_id].complete_cycle = curr_cycle;

								node[loc_id].used = TRUE;
								mac_check[rr][ee] = TRUE;

								output_loc_index[loc_pixel]++; //increase loc add order index
								loc_add[rr][ee][loc_check] = 0;
								break;
							}
						}
					}
				}
			}

			if (!mac_check[rr][ee])
			{
				int curr_mac_order = mac_order_index[rr][ee]; //mac order in each PE
				int mul_id = pe_mac_order_id[rr][ee][curr_mac_order];
				if (!node[mul_id].used) //if mac is not set
				{
					bool input_exist = FALSE;
					bool weight_exist = FALSE;
					bool output_exist = FALSE;

					for (int p_id1 = 0; p_id1 < NOP; p_id1++) //find parent pixels
					{
						if (node[mul_id].parentid[p_id1] < 0)
							continue;

						int id_1 = node[mul_id].parentid[p_id1];

						if (!strcmp(node[id_1].label, "ifmap"))
						{
							for (int qs = 0; qs < q * S; qs++)
							{
								if (node[input_reg[rr][ee][qs]].pixel == node[id_1].pixel)
								{
									if (node[input_reg[rr][ee][qs]].complete_cycle < curr_cycle) 
									{
										input_exist = TRUE;
										break;
									}
								}
							}
						}
						else if (!strcmp(node[id_1].label, "fmap"))
						{
							if (node[id_1].used == TRUE)
							{
								if (node[id_1].complete_cycle < curr_cycle)
								{
									weight_exist = TRUE;
								}
							}
						}
					}

					//if output spad is empty, then insert output id to output spad
					int spad_num = ((mac_order_index[rr][ee] - 1) % p);
					if (input_exist && weight_exist)
					{
						if (output_reg[rr][ee][spad_num] == 0)
						{
							for (int p_id1 = 0; p_id1 < NOP; p_id1++)
							{
								if (node[mul_id].parentid[p_id1] < 0)
									continue;

								int id_1 = node[mul_id].parentid[p_id1];

								if (!strcmp(node[id_1].label, "ofmap"))
								{
									output_reg[rr][ee][spad_num] = id_1;
									break;
								}
							}
						}
					}
					
					for (int p_id1 = 0; p_id1 < NOP; p_id1++)
					{
						if (node[mul_id].parentid[p_id1] < 0)
							continue;

						int id_1 = node[mul_id].parentid[p_id1];

						if (!strcmp(node[id_1].label, "ofmap"))
						{
							int p_output_id = id_1;
							if ((output_reg[rr][ee][spad_num] != 0) && ((node[output_reg[rr][ee][spad_num]].pixel) == (node[p_output_id].pixel)))
							{
								output_exist = TRUE;
								break;
							}
						}
					}

					if (input_exist && weight_exist && output_exist) //if parent pixels are ready, then set mac
					{
						node[mul_id].start_cycle = curr_cycle;
						node[mul_id].complete_cycle = curr_cycle;
						node[mul_id].used = TRUE;

						int child_id = node[mul_id].childid[0];

						node[child_id].start_cycle = node[mul_id].start_cycle;
						node[child_id].complete_cycle = node[mul_id].complete_cycle;
						node[child_id].used = TRUE;

						for (int p_id2 = 0; p_id2 < NOP; p_id2++)
						{
							if (node[mul_id].parentid[p_id2] < 0)
								continue;

							int id_2 = node[mul_id].parentid[p_id2];
							int id_2_pixel = node[id_2].pixel;

							if (!strcmp(node[id_2].label, "ifmap"))
							{
								for (int ww = 0; ww < W * q; ww++)
								{
									if (input_pixel_order[rr][ee][ww] == id_2_pixel)
									{
										input_pixel_count[rr][ee][ww]--; //decreases used pixel count in spad
																		 
										if (input_pixel_count[rr][ee][ww] == 0)
										{
											for (int qs = 0; qs < q*S; qs++)
											{
												int input_reg_id = input_reg[rr][ee][qs];
												if (input_pixel_order[rr][ee][ww] == node[input_reg_id].pixel)
												{
													input_reg[rr][ee][qs] = 0;
													break;
												}
											}
											break;
										}
										//break;
									}
								}
								//break;
							}
						}

						pe_mac_cnt[rr][ee][spad_num]++;
						if ((pe_mac_cnt[rr][ee][spad_num] != 0) && ((pe_mac_cnt[rr][ee][spad_num] % (q*S)) == 0))
						{
							local_link_mul_id[rr][ee][spad_num] = mul_id;
						}

						if (mac_order_index[rr][ee] < p * q * S * F)
						{
							mac_order_index[rr][ee]++; //increases mac order in each PE
						}
					}
				}
			}
		}
	}

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			//transfer output spad pixel (loc add, store)
			for (int pp = 0; pp < p; pp++)
			{
				int spad_pixel = node[output_reg[rr][ee][pp]].pixel;
				//output loc index : the number of local link add in one pixel
				if((local_link_mul_id[rr][ee][pp] != 0) && (output_loc_index[spad_pixel] == rr))
				{
					int ll_mul_id = local_link_mul_id[rr][ee][pp];
					int p_output_id;

					for (int p_id1 = 0; p_id1 < NOP; p_id1++)
					{
						if (node[ll_mul_id].parentid[p_id1] < 0)
							continue;

						int id_1 = node[ll_mul_id].parentid[p_id1];

						if (!strcmp(node[id_1].label, "ofmap"))
						{
							p_output_id = id_1;
							break;
						}
					}

					if (rr != R - 1)
					{
						int ll_child_id = node[ll_mul_id].ll_child_dependence;

						int child_r = node[ll_child_id].pe_info / E;
						int	child_e = node[ll_child_id].pe_info % E;
						
						if ((node[output_reg[child_r][child_e][pp]].pixel != node[output_reg[rr][ee][pp]].pixel))
							break;

						if ((rr == 0) && (node[p_output_id].used == FALSE))
						{
							break;
						}

						loc_add[child_r][child_e][pp] = output_reg[rr][ee][pp]; //loc add occur at next cycle

						output_reg[rr][ee][pp] = 0; //flush output spad pixel value
						pe_mac_cnt[rr][ee][pp] = 0;
						local_link_mul_id[rr][ee][pp] = 0;

						if (rr == 0) //first PE rows receive new output pixel from fifo
						{
							output_load_flag[ee][pp] = TRUE;
						}
					}
					else
					{
						if (node[output_reg[rr][ee][pp]].pixel != node[p_output_id].pixel) // if PE's output spad pixel is different with child PE's output spad
							break;

						if (node[p_output_id].used == FALSE)
							break;

						int child_id = node[ll_mul_id].childid[0];
						output_store_id[ee][pp] = node[child_id].childid[0]; //ready to store
					}
					break;
				}
			}
		}
	}
}

void load_from_input_fifo(nodeInfo *node, int input_pixel_order[R][E][W*q], int i_bank, int input_load_order_index[R][E], int input_reg[R][E][q * S], int **input_fifo, int curr_cycle)
{
	for (int ii = 0; ii < i_bank; ii++)
	{
		for (int rr = 0; rr < R; rr++)
		{
			for (int ee = 0; ee < E; ee++) //find input pixel in PE
			{
				if (input_load_order_index[rr][ee] < W * q) //if input load is not completed
				{
					for (int qs = 0; qs < q * S; qs++)
					{
						if ((input_reg[rr][ee][qs] == 0)) //find empty space in spad
						{
							for (int ff = 0; ff < (INPUT_FIFO_SIZE/i_bank) + 1; ff++)
							{
								int input_id = input_fifo[ii][ff];
								int pe_input_load_index = input_load_order_index[rr][ee];
								if ((input_fifo[ii][ff] != 0) && (input_pixel_order[rr][ee][pe_input_load_index] == node[input_id].pixel) && (pe_input_load_index < (W * q)))
								{
									if (node[input_id].complete_cycle < curr_cycle || node[input_id].used == TRUE)
									{
										input_reg[rr][ee][qs] = input_id;

										if (node[input_id].used == FALSE) //input can be loaded at different cycle
										{
											node[input_id].start_cycle = curr_cycle;
											node[input_id].complete_cycle = curr_cycle;
										}

										node[input_id].used = TRUE;
										input_load_order_index[rr][ee]++;

										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void load_to_input_fifo(nodeInfo *node, input_memory *i_mem, int i_pixel_id[H*W*C], int *i_index, int i_bank, int i_latency, bool *input_port_using, int *i_bank_number, int *i_port_using_comp_cycle, int **input_fifo, int curr_cycle)
{
	for (int ii = 0; ii < i_bank; ii++)
	{
		int i_mem_index = i_bank * i_index[ii] + ii;
		int i_mem_pixel = i_mem->order[i_mem_index];
		if ((i_mem_index < H * W * C) && (i_bank_number[i_mem_pixel] == ii)) //if input can be loaded
		{
			if (!input_port_using[ii]) //check port condition
			{
				for (int ff = 0; ff < (INPUT_FIFO_SIZE/i_bank) + 1; ff++)
				{
					if (input_fifo[ii][ff] == 0) //find empty space
					{
						int input_id = i_pixel_id[i_mem_pixel];

						input_port_using[ii] = TRUE;
						i_port_using_comp_cycle[ii] = curr_cycle + i_latency - 1;
						input_fifo[ii][ff] = input_id;

						node[input_id].start_cycle = curr_cycle;
						node[input_id].complete_cycle = curr_cycle + i_latency - 1;

						i_index[ii]++;
						
						break;
					}
				}
			}
		}
	}
}

void load_from_weight_fifo(nodeInfo *node, int weight_pixel_order[R][p * q * S], int weight_load_order_index[R], int w_bank, int **weight_fifo, int curr_cycle)
{
	for (int ww = 0; ww < w_bank; ww++)
	{
		for (int rr = 0; rr < R; rr++)
		{
			if (weight_load_order_index[rr] < p * q * S) //if weight load is not completed
			{
				for (int ff = 0; ff < (WEIGHT_FIFO_SIZE/w_bank) + 1; ff++)
				{
					int weight_id = weight_fifo[ww][ff];
					if ((weight_fifo[ww][ff] != 0) && (node[weight_id].used == FALSE) && ((node[weight_id].pixel == weight_pixel_order[rr][weight_load_order_index[rr]])))
					{
						if (node[weight_id].complete_cycle < curr_cycle)
						{
							node[weight_id].start_cycle = curr_cycle;
							node[weight_id].complete_cycle = curr_cycle;

							node[weight_id].used = TRUE;

							weight_load_order_index[rr]++;
							break;
						}
					}
				}
			}
		}
	}
}	

void load_to_weight_fifo(nodeInfo *node, weight_memory *w_mem, int w_pixel_id[R*S*C*M], int *w_index, int w_bank, int w_latency, bool *weight_port_using, int *w_bank_number, int *w_port_using_comp_cycle, int **weight_fifo, int curr_cycle)
{
	for (int ww = 0; ww < w_bank; ww++)
	{
		int w_mem_index = w_bank * w_index[ww] + ww;
		int w_mem_pixel = w_mem->order[w_mem_index];
		if ((w_mem_index < R * S * C * M) && (w_bank_number[w_mem_pixel] == ww)) //if weight can be loaded to fifo
		{
			if (!weight_port_using[ww])
			{
				for (int ff = 0; ff < (WEIGHT_FIFO_SIZE/w_bank) + 1; ff++)
				{
					if (weight_fifo[ww][ff] == 0)
					{
						int weight_id = w_pixel_id[w_mem_pixel];
					
						weight_port_using[ww] = TRUE;
						w_port_using_comp_cycle[ww] = curr_cycle + w_latency - 1;
						weight_fifo[ww][ff] = weight_id;
						
						node[weight_id].start_cycle = curr_cycle;
						node[weight_id].complete_cycle = curr_cycle + w_latency - 1;
						
						w_index[ww]++;
						
						break;
					}
				}
			}
		}
	}
}

void load_from_output_fifo(nodeInfo *node, int o_bank, int output_reg[R][E][p], int output_pixel_order[E][F * p], int output_load_order_index[E], bool *is_output_loading, int *output_load_comp_cycle, int **output_fifo, bool output_load_flag[E][p], int curr_cycle)
{
	for (int oo = 0; oo < o_bank; oo++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			if (output_load_order_index[ee] < p * F) //if output load is not completed
			{
				for (int ff = 0; ff < (OUTPUT_FIFO_SIZE/o_bank) + 1; ff++)
				{
					int output_id = output_fifo[oo][ff];
					int pp = (node[output_id].pixel / (E*F)); //spad location
					if (output_load_flag[ee][pp]) //output_load_flag becomes TRUE when local link occurred at first PE row
					{
						if ((output_fifo[oo][ff] != 0) && (node[output_id].used == FALSE) && (node[output_id].pixel == output_pixel_order[ee][output_load_order_index[ee]]))
						{
							if (node[output_id].complete_cycle < curr_cycle)
							{
								node[output_id].start_cycle = curr_cycle;
								node[output_id].complete_cycle = curr_cycle;

								node[output_id].used = TRUE;

								output_reg[0][ee][pp] = output_id;

								is_output_loading[ee] = TRUE; //pe cannot set MAC while pe is loading output pixel
								output_load_comp_cycle[ee] = curr_cycle;

								output_load_order_index[ee]++;

								output_load_flag[ee][pp] = FALSE;

								break;
							}
						}
					}
				}
			}
		}
	}
}

void load_to_output_fifo(nodeInfo *node, output_memory *o_ldr_mem, int o_load_pixel_id[E*F*M], int *o_load_index, int o_bank, int o_latency, bool *o_load_port_using, int *o_bank_number, int *o_load_port_comp_cycle, int **output_fifo, int curr_cycle)
{
	for (int oo = 0; oo < o_bank; oo++)
	{
		int o_mem_index = o_bank * o_load_index[oo] + oo;
		int o_mem_pixel = o_ldr_mem->order[o_mem_index];
		if ((o_mem_index < E * F * M) && (o_bank_number[o_mem_pixel] == oo)) //if output can be loaded to fifo
		{
			if (!o_load_port_using[oo])
			{
				for (int ff = 0; ff < (OUTPUT_FIFO_SIZE/o_bank) + 1; ff++)
				{
					if (output_fifo[oo][ff] == 0)
					{
						int output_id = o_load_pixel_id[o_mem_pixel];

						o_load_port_using[oo] = TRUE;
						o_load_port_comp_cycle[oo] = curr_cycle + o_latency - 1;

						node[output_id].start_cycle = curr_cycle;
						node[output_id].complete_cycle = curr_cycle + o_latency - 1;

						output_fifo[oo][ff] = output_id;
						
						o_load_index[oo]++;

						break;
					}
				}
			}
		}
	}
}

void store_output(nodeInfo *node, int o_bank, int o_latency, int output_reg[R][E][p], int *o_bank_number, bool *o_store_port_using, int *o_store_port_comp_cycle, int pe_mac_cnt[R][E][p], int local_link_mul_id[R][E][p], int output_store_id[E][p], int curr_cycle)
{
	for (int ee = 0; ee < E; ee++) // PE at the bottom 
	{
		for (int pp = 0; pp < p; pp++)
		{
			if (output_store_id[ee][pp] > 0) //check output store flag
			{
				for (int oo = 0; oo < o_bank; oo++)
				{
					if (node[output_store_id[ee][pp]].used == FALSE) //find pixel location to store
					{
						int o_mem_pixel = node[output_store_id[ee][pp]].pixel;
						if (o_bank_number[o_mem_pixel] == oo)
						{
							if ((!o_store_port_using[oo])) // check port condition
							{
								node[output_store_id[ee][pp]].start_cycle = curr_cycle + 1;
								node[output_store_id[ee][pp]].complete_cycle = curr_cycle + o_latency;
								node[output_store_id[ee][pp]].used = TRUE;

								o_store_port_using[oo] = TRUE;
								o_store_port_comp_cycle[oo] = curr_cycle + o_latency;

								output_reg[R - 1][ee][pp] = 0;
								pe_mac_cnt[R - 1][ee][pp] = 0;
								local_link_mul_id[R - 1][ee][pp] = 0;

								output_store_id[ee][pp] = 0;

								break;
							}
						}
					}
				}
			}
		}
	}
}

void unlock_port_constraint(int bank_num, bool *port_using, int *port_using_comp_cycle, int curr_cycle)
{
	for (int pp = 0; pp < bank_num; pp++) //unlock port constraint
	{
		if (curr_cycle >= port_using_comp_cycle[pp])
			port_using[pp] = FALSE;
	}
}

void unlock_mac_constraint_by_load(bool *is_output_loading, int *output_load_comp_cycle, int curr_cycle)
{
	for (int ee = 0; ee < E; ee++) //unlock mac constraint by load
	{
		if (curr_cycle >= output_load_comp_cycle[ee])
			is_output_loading[ee] = FALSE;
	}
}

int mem_latency_bank_constraint(nodeInfo *node, int pe_mac_order_id[R][E][p * q * S * F + 1], int last_node_id, unsigned int i_latency, unsigned int w_latency, unsigned int o_latency, unsigned int i_bank, unsigned int w_bank, unsigned int o_bank, input_memory *i_mem, weight_memory *w_mem, output_memory *o_ldr_mem, output_memory *o_str_mem, int i_pixel_id[H*W*C], int w_pixel_id[R*S*C*M], int o_pixel_id[E*F*M])
{
	int curr_cycle = 0;
	int max_cycle = 0;

	bool *input_port_using = (bool *)calloc(i_bank, sizeof(bool));
	bool *weight_port_using = (bool *)calloc(w_bank, sizeof(bool));
	bool *o_load_port_using = (bool *)calloc(o_bank, sizeof(bool));
	bool *o_store_port_using = (bool *)calloc(o_bank, sizeof(bool));

	int *i_port_using_comp_cycle = (int *)calloc(i_bank, sizeof(int));
	int *w_port_using_comp_cycle = (int *)calloc(w_bank, sizeof(int));
	int *o_load_port_comp_cycle = (int *)calloc(o_bank, sizeof(int));
	int *o_store_port_comp_cycle = (int *)calloc(o_bank, sizeof(int));

	int i_bank_number[H*W*C];
	int w_bank_number[R*S*C*M];
	int o_bank_number[E*F*M];

	int *i_index = (int *)calloc(i_bank, sizeof(int));
	int *w_index = (int *)calloc(w_bank, sizeof(int));
	int *o_load_index = (int *)calloc(o_bank, sizeof(int));

	int input_pixel_count[R][E][W*q];
	int input_pixel_order[R][E][W*q];
	int input_load_order_index[R][E] = { 0, };
	int output_pixel_order[E][F * p] = { 0, };
	int output_load_order_index[E] = { 0, };
	int output_loc_index[E*F*M] = { 0, };
	int weight_pixel_order[R][p * q * S] = { 0, };
	int weight_load_order_index[R] = { 0, };

	int mac_order_index[R][E];
	int pe_mac_cnt[R][E][p] = { 0, };
	int local_link_mul_id[R][E][p] = { 0, };
	int loc_add[R][E][p] = { 0, };
	bool mac_check[R][E] = { 0, };

	int input_reg[R][E][q * S] = { 0, };
	int output_reg[R][E][p] = { 0, };
	
	bool is_output_loading[E] = { 0, };
	int output_load_comp_cycle[E] = { 0, };
	bool output_load_flag[E][p] = { 0, };
	int output_store_id[E][p] = { 0, };

	int **input_fifo = (int **)calloc(i_bank,sizeof(int));
	int **weight_fifo = (int **)calloc(w_bank, sizeof(int));
	int **output_fifo = (int **)calloc(o_bank, sizeof(int));

	//set dynamic memory
	initialize_fifo_mem(input_fifo, INPUT_FIFO_SIZE, i_bank);
	initialize_fifo_mem(weight_fifo, WEIGHT_FIFO_SIZE, w_bank);
	initialize_fifo_mem(output_fifo, OUTPUT_FIFO_SIZE, o_bank);

	//make all node to unused status
	initialize_node_status(node);

	update_input_pixel_count(input_pixel_order, input_pixel_count, i_mem);//update_pixel_count
	update_weight_pixel_order(weight_pixel_order, w_mem);
	update_output_pixel_order(output_pixel_order, o_ldr_mem);

	//set output load flag(first PE row can load output at the first time)
	initialize_output_load_flag(output_load_flag);	
	
	//initialize mac order index(mac order starts from 1 in each PE)
	initialize_mac_order_index(mac_order_index);

	//set pixel where to use(pixel to bank)
	update_i_pixel_to_bank(i_bank_number, H*W*C, i_bank, i_mem);
	update_w_pixel_to_bank(w_bank_number, R*S*C*M, w_bank, w_mem);
	update_o_pixel_to_bank(o_bank_number, E*F*M, o_bank, o_ldr_mem);

	while (1)
	{
		//initialize mac_check to FALSE (if mac_check is TRUE -> cannot set MAC)
		initialize_mac(mac_check);

		//load output from fifo to 
		load_from_output_fifo(node, o_bank, output_reg, output_pixel_order, output_load_order_index, is_output_loading, output_load_comp_cycle, output_fifo, output_load_flag, curr_cycle);
		
		//set mac
		update_constraint_mac(node, pe_mac_order_id, pe_mac_cnt, input_reg, input_pixel_order, input_pixel_count, output_reg, mac_check, local_link_mul_id, loc_add, mac_order_index, output_store_id, is_output_loading, output_loc_index, output_load_flag, curr_cycle);
		
		//input, weight load to spad
		load_from_input_fifo(node, input_pixel_order, i_bank, input_load_order_index, input_reg, input_fifo, curr_cycle);
		load_from_weight_fifo(node, weight_pixel_order, weight_load_order_index, w_bank, weight_fifo, curr_cycle);

		//GLB to fifo
		load_to_input_fifo(node, i_mem, i_pixel_id, i_index, i_bank, i_latency, input_port_using, i_bank_number, i_port_using_comp_cycle, input_fifo, curr_cycle);
		load_to_weight_fifo(node, w_mem, w_pixel_id, w_index, w_bank, w_latency, weight_port_using, w_bank_number, w_port_using_comp_cycle, weight_fifo, curr_cycle);
		load_to_output_fifo(node, o_ldr_mem, o_pixel_id, o_load_index, o_bank, o_latency, o_load_port_using, o_bank_number, o_load_port_comp_cycle, output_fifo, curr_cycle);
		
		//set store
		store_output(node, o_bank, o_latency, output_reg,  o_bank_number, o_store_port_using, o_store_port_comp_cycle, pe_mac_cnt, local_link_mul_id, output_store_id, curr_cycle);

		//unlock port constraint
		unlock_port_constraint(i_bank, input_port_using, i_port_using_comp_cycle, curr_cycle);
		unlock_port_constraint(w_bank, weight_port_using, w_port_using_comp_cycle, curr_cycle);
		unlock_port_constraint(o_bank, o_load_port_using, o_load_port_comp_cycle, curr_cycle);
		unlock_port_constraint(o_bank, o_store_port_using, o_store_port_comp_cycle, curr_cycle + 1);

		//unlock mac constraint by load
		unlock_mac_constraint_by_load(is_output_loading, output_load_comp_cycle, curr_cycle);

		curr_cycle++;

		if ((node[last_node_id].used == TRUE) && (node[last_node_id].complete_cycle < curr_cycle))
			break;

		if (curr_cycle > MAX_CYCLE)
		{
			cout << "error" << endl;
			cout << "(" << i_latency << " " << w_latency << " " << o_latency << "), (" << i_bank << " " << w_bank << " " << o_bank << ")" << endl;
			print_dddg(node, MAX_CYCLE);
			int stop;
			cin >> stop;
			break;
		}
	}

	int input_num = 0;
	int weight_num = 0;
	int output_num = 0;
	int mac_num = 0;
	int loc_num = 0;
	int store_num = 0;

	for (int id = 0; id < NODE_NUM + NODE_NUM_LOC; id++)
	{
		if (node[id].used == FALSE)
			continue;

		if (!strcmp(node[id].microop, "Load"))
		{
			if (!strcmp(node[id].label, "ifmap"))
				input_num++;
			else if (!strcmp(node[id].label, "fmap"))
				weight_num++;
			else if (!strcmp(node[id].label, "ofmap"))
				output_num++;
		}
		else if (!strcmp(node[id].microop, "Mul"))
			mac_num++;
		else if (!strcmp(node[id].microop, "Store"))
			store_num++;
		else if (!strcmp(node[id].microop, "LOC_ADD"))
			loc_num++;
	}
	
	if ((input_num != (H*W*C)) && (weight_num != (R*S*C*M)) && (output_num != (E*F*M)) && (mac_num != (p*q*E*F*R*S)) && (store_num != (E*F*M)) && (loc_num != (p * (R - 1) * E * F)))
	{
		cout << "error" << endl;
		cout << "(" << i_latency << " " << w_latency << " " << o_latency << "), (" << i_bank << " " << w_bank << " " << o_bank << ")" << endl;
		print_dddg(node, MAX_CYCLE);
		int stop;
		cin >> stop;
	}

	find_max_cycle(node, &max_cycle);

	free(input_port_using);
	free(weight_port_using);
	free(o_load_port_using);
	free(o_store_port_using);

	free(i_port_using_comp_cycle);
	free(w_port_using_comp_cycle);
	free(o_load_port_comp_cycle);
	free(o_store_port_comp_cycle);

	free(i_index);
	free(w_index);
	free(o_load_index);

	free_fifo_mem(input_fifo, i_bank);
	free_fifo_mem(weight_fifo, w_bank);
	free_fifo_mem(output_fifo, o_bank);
	
	return max_cycle;
}
