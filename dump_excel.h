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

#include <string>
typedef struct _mac_count_for_local_link
{
	int count;
	int pixel;
} mac_count_for_local_link;

void dump_graph(nodeInfo *node, int cycle)
{
	FILE *fp;
	int r_index = 0;
	int e_index = 0;
	int i_first = 0;
	int w_first = 0;
	int o_first = 0;
	int str_first = 0;

	char tmp_char[200];

	mac_count_for_local_link mac_count[p];
	string temp[5];
	fopen_s(&fp, "dump_graph.txt", "w");

	fprintf_s(fp, "cycle\t\t");

	for (e_index = 0; e_index < E; e_index++)
	{
		for (r_index = 0; r_index < R; r_index++)
		{
		
			fprintf_s(fp, "mac\tinput_in\tweight_in\toutput_in\toutput_out\t");

		}
	}
	for (int i = 0; i < 5; i++) //initialize
	{
		temp[i].assign("");
		temp[i].assign("\t");
	}




	fprintf_s(fp, "\n");
	for (int cc = 0; cc < cycle; cc++)
	{
		fprintf_s(fp, "%d\t\t", cc);

		for (e_index = 0; e_index < E; e_index++)
		{
			for (r_index = 0; r_index < R; r_index++)
			{
				for (int id = 0; id < NODE_NUM + NODE_NUM_LOC; id++)
				{
					if (*node[id].microop == NULL)
						continue;

					if (cc == node[id].complete_cycle)
					{
						if (!strcmp(node[id].microop, "Mul"))// || !strcmp(node[id].microop, "Add")) //FIND MAC
						{
							if (node[id].pe_info == E * r_index + e_index) // find correct PE
							{
								for (int ii = 0; ii < NOP; ii++)
								{
									int KK = 0;
									KK = node[id].parentid[ii];
									if (KK < 0)
										continue;
									if (!strcmp(node[KK].label, "ifmap"))
									{
										temp[0].assign("I");
										temp[0].append(std::to_string(node[KK].pixel));
										temp[0].append(",");
									}
									if (!strcmp(node[KK].label, "fmap"))
									{
										temp[0].append("F");
										temp[0].append(std::to_string(node[KK].pixel));
										temp[0].append(",");
									}
									if (!strcmp(node[KK].label, "ofmap"))
									{
										temp[0].append("O");
										temp[0].append(std::to_string(node[KK].pixel));
										temp[0].append("\t");
									}
								}
							}

						}

						if (!strcmp(node[id].microop, "Load"))
						{
							if (!strcmp(node[id].label, "ifmap"))
							{
								if (node[id].pixel % (H*W) / H == r_index + e_index) {

									temp[1].assign("I");
									temp[1].append(std::to_string(node[id].pixel));
									temp[1].append("\t");


								}


							}


							if (!strcmp(node[id].label, "fmap"))
							{
								
								if (node[id].pixel % (R*S) / R == r_index)
								{

									temp[2].assign("F");
									temp[2].append(std::to_string(node[id].pixel));
									temp[2].append("\t");

								}

							}


							if (!strcmp(node[id].label, "ofmap"))
							{
								
								if (node[id].pixel % (E*F) / E == E*r_index+e_index)
								{

									temp[3].assign("O");
									temp[3].append(std::to_string(node[id].pixel));
									temp[3].append("\t");
								}

							}


						}

						if (!strcmp(node[id].microop, "LOC_ADD"))
						{

							if (node[id].pe_info == E * r_index + e_index)
							{
								//printf("locadd!\n");

								temp[3].assign("O");
								temp[3].append(std::to_string(node[id].pixel));
								temp[3].append("\t");

							}

						}

						if (!strcmp(node[id].microop, "Store"))
						{

								if (E*(R-1)+node[id].pixel % (E*F) / E == E*r_index+e_index)
								{

									temp[4].assign("O");
									temp[4].append(std::to_string(node[id].pixel));
									temp[4].append("\t");
								}

						}

					}
				}

				for (int i = 0; i <5; i++) //initialize
				{
					fprintf_s(fp, "%s", temp[i].c_str());
					temp[i].assign("");
					temp[i].append("\t");
				}

			}
		}
		fprintf_s(fp, "\n");

	}

	fclose(fp);
 }
