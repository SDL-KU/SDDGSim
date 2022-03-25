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

void output_load_dependence(nodeInfo *node, int *cycle)
{
	//int tmp;

	for (int cc = 0; cc < *cycle; cc++)
	{
		if (cc == 0)
			continue;

		for (int id = 0; id < NODE_NUM; id++)
		{
			if (*node[id].microop == NULL)
				continue;

			if (!strcmp(node[id].microop, "Load"))
			{
				if (!strcmp(node[id].label, "ofmap"))
				{
					if (node[id].complete_cycle == cc)
					{
						for (int id2 = 0; id2 < NODE_NUM; id2++)
						{
							if (*node[id2].microop == NULL)
								continue;

							if (node[id2].complete_cycle == cc - 1)
							{
								if (!strcmp(node[id2].microop, "Add"))
								{
									for (int pid = 0; pid < NOP; pid++)
									{

										if (node[id2].parentid[pid] == INITIALIZE)
											continue;

										int parent_id = node[id2].parentid[pid];

										if (!strcmp(node[parent_id].microop, "Load"))
										{
											if (!strcmp(node[parent_id].label, "ofmap"))
											{
												int prev_pixel = node[parent_id].pixel;

												if ((node[id].pixel - 1) == prev_pixel)
												{
													node[id].parentid[0] = id2;
													node[id2].childid[0] = id;
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
		}
	}
}

//void set_node_to_pe_dependence(nodeInfo *node)
//{
//	int MUL_ORDER = 1;
//	int ADD_ORDER = 1;
//	int tmp = 0;
//
//	//cout << "SET_NODE_TO_PE_DEPENDENCE" << endl;
//
//	for (int PE = 0; PE < R*E; PE++)
//	{
//		for (int curr_cycle = 0; curr_cycle < MAX_CYCLE; curr_cycle++)
//		{
//			for (int id = 0; id < NODE_NUM; id++)
//			{
//				int PE_NUMBER = PE;
//
//				if (*node[id].microop == NULL)
//					continue;
//
//				if (!strcmp(node[id].microop, "Mul"))
//				{
//					if (PE_NUMBER == node[id].pe_info)
//					{
//						if (curr_cycle == node[id].complete_cycle)
//						{
//							node[id].mac_order = MUL_ORDER;
//							MUL_ORDER++;
//
//							if (MUL_ORDER > E*S*q)
//								MUL_ORDER = 1;
//						}
//					}
//				}
//				else if (!strcmp(node[id].microop, "Add"))// (!strcmp(node[id].microop, "Add"))
//				{
//					if (PE_NUMBER == node[id].pe_info)
//					{
//						if (curr_cycle == node[id].complete_cycle)
//						{
//							node[id].mac_order = ADD_ORDER;
//							ADD_ORDER++;
//
//							if (ADD_ORDER > E*S*q)
//								ADD_ORDER = 1;
//						}
//					}
//				}
//				else
//				{
//					continue;
//				}
//
//			}
//		}
//	}


//for (int PE = 0; PE < R*E; PE++)
//{
//	//MUL_ORDER = 1;
//	//ADD_ORDER = 1;
//	for (int id = 0; id < NODE_NUM; id++)
//	{
//		int PE_NUMBER = PE;
//
//		if (*node[id].microop == NULL)
//			continue;
//
//		if (!strcmp(node[id].microop, "Mul"))
//		{
//
//			if (PE_NUMBER == node[id].pe_info)
//			{
//				node[id].mac_order = MUL_ORDER;
//				MUL_ORDER++;
//
//				cout <<"ID: "<< id << endl;
//				cout << "PE: " << node[id].pe_info << endl;
//				cout << "MO: " << node[id].mac_order << endl;
//				cin >> tmp;
//
//				if (MUL_ORDER > E*S*q)
//					MUL_ORDER = 1;
//			}
//		}
//		else if (!strcmp(node[id].microop, "Add"))// (!strcmp(node[id].microop, "Add"))
//		{
//			if (PE_NUMBER == node[id].pe_info)
//			{
//				node[id].mac_order = ADD_ORDER;
//				ADD_ORDER++;
//
//				if (ADD_ORDER > E*S*q)
//					ADD_ORDER = 1;
//			}
//		}
//		else
//		{
//			continue;
//		}
//	}
//}

// CHECK
/*for (int id = 0; id < NODE_NUM; id++)
{
if (*node[id].microop == NULL)
continue;

if (!strcmp(node[id].microop, "Mul"))
{
cout <<"ID: "<< id << endl;
cout << "MICRO OP: " << node[id].microop << endl;
cout << "PE NUMBER: " << node[id].pe_info << endl;
cout << "MUL ORDER: " << node[id].mac_order << endl;

for (int pid = 0; pid < 10; pid++)
{
cout <<"PID: "<< node[id].parentid[pid] << endl;
}

for (int cid = 0; cid < 10; cid++)
{
cout << "CID: " << node[id].childid[cid] << endl;
}
}
else if (!strcmp(node[id].microop, "Add"))
{
cout << "ID: " << id << endl;
cout << "MICRO OP: " << node[id].microop << endl;
cout << "PE NUMBER: " << node[id].pe_info << endl;
cout << "Add ORDER: " << node[id].mac_order << endl;

for (int pid = 0; pid < 10; pid++)
{
cout << "PID: " << node[id].parentid[pid] << endl;
}

for (int cid = 0; cid < 10; cid++)
{
cout << "CID: " << node[id].childid[cid] << endl;
}
}
else
{
continue;
}

cin >> tmp;
}
}*/

void local_link_dependence_update(nodeInfo *node)
{
	int p_index = 0;
	int c_index = 0;
	int tmp;

	for (int PE = 0; PE < R*E; PE++)
	{
		//if (PE < E)
		//	continue;

		for (int id = 0; id < NODE_NUM; id++)
		{
			if (*node[id].microop == NULL)
				continue;

			if (!strcmp(node[id].microop, "Mul")) // CHILD PE
			{
				if (node[id].pe_info == PE)
				{
					if (((node[id].mac_order - 1) % (p * q *S)) >= ((p * q * S) - p))
					{
						for (int id2 = 0; id2 < NODE_NUM; id2++)
						{
							if (*node[id2].microop == NULL)
								continue;

							if (!strcmp(node[id2].microop, "Mul")) // PARENT PE
							{
								if (node[id2].pe_info == (node[id].pe_info - E))
								{
									if (node[id].mac_order == node[id2].mac_order)
									{
										node[id2].ll_child_dependence = id;
								 		node[id].ll_parent_dependence = id2;
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
