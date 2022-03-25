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

int set_iwo_order(nodeInfo *node, int *order, int(*extra_nodes)[100], char *ref_name, int max_cycle, unsigned int port, unsigned int latency, unsigned int pixel_size)
{
	int id = 0;
	int max = 0;
	int index = 0;
	int max_cycle_tmp = 0;
	bool *load_flag;
	int SIZE = pixel_size;
	int KK;

	load_flag = (bool*)calloc(SIZE, sizeof(bool));
	int bound = 0;

	// Initialize
	for (int ii = 0; ii < SIZE; ii++) {
		load_flag[ii] = FALSE;
	}

	//Set cycle
	
	for (index = 0; index < SIZE; index++)
	{
		bound = 0;
		for (id = 0; id < NODE_NUM; id++)
		{
			if (!strcmp(node[id].microop, "Load"))
			{
				if (!strcmp(node[id].label, ref_name))
				{
					if (node[id].pixel == order[index])
					{
						if (load_flag[node[id].pixel] == FALSE)
						{
							node[id].start_cycle = (index / port)*(1 + latency); //order_cycle;
							node[id].complete_cycle = (index / port)*(1 + latency) + latency; //order_cycle + LATENCY;																			
							load_flag[node[id].pixel] = TRUE;
						}
						else
						{
							extra_nodes[node[id].pixel][bound] = id;
							node[id].start_cycle = -1;
							node[id].complete_cycle = -1;
							bound++;
						}
					}
				}
			}
		}
	}

	/*
	int cc = 0;
	for (int k = 0; k < NODE_NUM; k++) {
		if (!(strcmp(node[k].label, ref_name)))
		{
			if (node[k].complete_cycle == cc) {
				cout << "node id : " << k << endl;
				cout << node[k].pixel << endl;
				cout << node[k].complete_cycle << endl;
				cin >> KK;
				cc++;
				k = 0;
			}
		}
	}
	*/

	check_max_cycle(node, &max_cycle_tmp);

	max = (max_cycle >= max_cycle_tmp) ? max_cycle : max_cycle_tmp;

	free(load_flag);

	return max;
}