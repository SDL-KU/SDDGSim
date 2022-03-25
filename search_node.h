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

using namespace std;

int find_load_node_input(nodeInfo *node, int index, char *ref_name, input_memory *i_mem, int cycle, bool OK) 
{
	int node_id = 0;

	if (OK == FALSE)
	{
		for (int ID = 0; ID < NODE_NUM; ID++)
		{
			if (node[ID].used == TRUE)
				continue;

			if (!strcmp(node[ID].microop, "Load"))
			{
				if (!strcmp(node[ID].label, ref_name))
				{
					if (node[ID].pixel == i_mem->order[index])
					{
						node_id = ID;
						node[ID].start_cycle = cycle;
						node[ID].complete_cycle = cycle;
						node[ID].used = TRUE;
						break;
					}
				}
			}
		}
	}
	else
	{
		node_id = -999;
	}

	return node_id;
}

int find_load_node_weight(nodeInfo *node, int index, char *ref_name, weight_memory *w_mem, int cycle, bool OK)
{
	int node_id = 0;

	if (OK == FALSE)
	{
		for (int ID = 0; ID < NODE_NUM; ID++)
		{
			//if (node[ID].used == TRUE)
			//	continue;

			if (!strcmp(node[ID].microop, "Load"))
			{
				if (!strcmp(node[ID].label, ref_name))
				{
					if (node[ID].pixel == w_mem->order[index])
					{
						node_id = ID;
						//node[ID].start_cycle = cycle;
						//node[ID].complete_cycle = cycle;
						//node[ID].used = TRUE;
						node[ID].already_load = TRUE;
						break;
					}
				}
			}
		}
	}
	else
	{
		node_id = -999;
	}

	return node_id;
}

int find_load_node_output(nodeInfo *node, int index, char *ref_name, output_memory *o_mem, int cycle, bool OK)
{
	int node_id = 0;

	if (OK == FALSE)
	{
		for (int ID = 0; ID < NODE_NUM; ID++)
		{
			if (!strcmp(node[ID].microop, "Load"))
			{
				if (!strcmp(node[ID].label, ref_name))
				{
					if (node[ID].pixel == o_mem->order[index])
					{
						node_id = ID;
						//node[ID].start_cycle = cycle;
						//node[ID].complete_cycle = cycle;
						//node[ID].used = TRUE;
						node[ID].already_load = TRUE;
						break;
					}
				}
			}
		}
	}
	else
	{
		node_id = -999;
	}
	
	return node_id;
}

int find_store_node_output(nodeInfo *node, int index, char *ref_name, output_memory *o_mem_str, bool output_order_complete)
{
	int node_id = 0;

	if (output_order_complete == FALSE)
	{
		for (int ID = 0; ID < NODE_NUM; ID++)
		{
			if (!strcmp(node[ID].microop, "Store"))
			{
				if (!strcmp(node[ID].label, ref_name))
				{
					if (node[ID].pixel == o_mem_str->order[index])
					{
						node_id = ID;
						node[ID].used = TRUE;
					}
				}
			}
		}
	}
	else
	{
		node_id = -999;
	}

	return node_id;
}

void reschedule_str_node(nodeInfo *node)
{
	int previous_id = 0;
	int previous_pixel = 0;
	int match_cnt = 0;

	for (int cycle = 0; cycle < MAX_CYCLE; cycle++)
	{
		previous_id = 0;
		previous_pixel = 0;
		match_cnt = 0;

		for (int id = 0; id < NODE_NUM; id++)
		{
			if (!strcmp(node[id].microop, "Store"))
			{
				if (node[id].complete_cycle == cycle)
				{
					if (node[id].used == TRUE)
					{
						if (match_cnt == 0)
						{
							previous_id = id;
							previous_pixel = node[id].pixel;
							match_cnt++;
						}
						else
						{
							if (previous_pixel < node[id].pixel)
							{
								node[id].start_cycle = node[id].start_cycle + 1;
								node[id].complete_cycle = node[id].complete_cycle + 1;
							}
							else
							{
								node[previous_id].start_cycle = node[previous_id].start_cycle + 1;
								node[previous_id].complete_cycle = node[previous_id].complete_cycle + 1;
								previous_id = id;
								previous_pixel = node[id].pixel;
							}
							match_cnt++;
						}
					}
				}
			}
		}
	}

}
