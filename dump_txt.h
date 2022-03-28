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

void load_text(nodeInfo *node, int *last_node_id)
{
	FILE *node_info_file;
	int tmp = 0;
	char LOC_NODE[100]  = "LOC_ADD" ;

	for (unsigned int i = 0; i < NODE_NUM + NODE_NUM_LOC; i++) 
	{
		if (i > NODE_NUM - 1)
		{
			strcpy(node[i].microop, LOC_NODE);
			node[i].start_cycle = -1;
			node[i].complete_cycle = -1;
			*node[i].label = NULL;
			node[i].pixel = -1;
			*node[i].childid = NULL;
			*node[i].parentid = NULL;
			for (int j = 0; j < NOP; j++) { // NOP is number of parents
				node[i].parentid[j] = INITIALIZE;
				node[i].childid[j] = INITIALIZE;
			}
			node[i].used = FALSE;
			node[i].output_used = FALSE;

			for (int rr = 0; rr<R; rr++)
				node[i].row_pe[rr] = INITIALIZE;

			for (int ee = 0; ee<E; ee++)
				node[i].col_pe[ee] = INITIALIZE;
		}
		else
		{
			*node[i].microop = NULL;
			node[i].start_cycle = NULL;
			node[i].complete_cycle = NULL;
			*node[i].label = NULL;
			node[i].pixel = -1;
			*node[i].childid = NULL;
			*node[i].parentid = NULL;
			for (int j = 0; j < NOP; j++) { // NOP is number of parents
				node[i].parentid[j] = INITIALIZE;
				node[i].childid[j] = INITIALIZE;
			}
			node[i].used = FALSE;
			node[i].output_used = FALSE;

			for (int rr = 0; rr<R; rr++)
				node[i].row_pe[rr] = INITIALIZE;

			for (int ee = 0; ee<E; ee++)
				node[i].col_pe[ee] = INITIALIZE;
		}
	}

	//node_info_file = fopen("nodeinfo_new_3x3_M2_C2.txt", "r");
	//node_info_file = fopen("nodeinfo_new_3x13_M4_C4.txt", "r");
	//node_info_file = fopen("nodeinfo_newnew_3x3_M2C2.txt", "r");
	//node_info_file = fopen("nodeinfo_final_3x3_M2C2.txt", "r");
	node_info_file = fopen("M16_C4.txt", "r");
	//node_info_file = fopen("AlexNet_Layer_Ex/AlexNet_Layer5.txt", "r");


	char temp_info[NODE_NUM] = { 0, };
	int node_id = 0;
	int check = 0;
	int counter = 0;

	while (!feof(node_info_file)) 
	{
		fscanf(node_info_file, "%s", temp_info);
		node_id = atoi(temp_info);
		if (node_id == EOFF)
			break;
		fscanf(node_info_file, "%s", node[node_id].microop);
		fscanf(node_info_file, "%s", temp_info);
		node[node_id].start_cycle = -1;
		fscanf(node_info_file, "%s", temp_info);
		node[node_id].complete_cycle = -1;
		fscanf(node_info_file, "%s", node[node_id].label);
		fscanf(node_info_file, "%s", temp_info);
		node[node_id].pixel = atoi(temp_info);
		int i = 0;
		fscanf(node_info_file, "%s,", temp_info);

		while (strcmp(temp_info, "-")) 
		{
			if (atoi(temp_info) != BLANK) 
			{
				node[node_id].childid[i] = atoi(temp_info);
				i++;
				fscanf(node_info_file, "%s,", temp_info);
			}
			else
				break;
		}

		i = 0;

		fscanf(node_info_file, "%s,", temp_info);
		while (strcmp(temp_info, "-")) {
			if (atoi(temp_info) != BLANK) {
				node[node_id].parentid[i] = atoi(temp_info);
				i++;
				fscanf(node_info_file, "%s,", temp_info);
			}
			else
				break;
		}

		fscanf(node_info_file, "%s,", temp_info);

		node[node_id].time_dependence = FALSE;

		(*last_node_id)++;
		if (atoi(temp_info) == BLANK)
			continue;
		else if (atoi(temp_info) == 6000)
			break;
	}

	fclose(node_info_file);

	cout << "last_node_id :" << *last_node_id << endl;
}
