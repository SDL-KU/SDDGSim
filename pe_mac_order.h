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

void set_node_to_pe_dependence(nodeInfo *node, int pe_mac_order_id[R][E][p*q*S*F+1], int standard_DDDG_cycle)
{
	int MAC_ORDER = 1;
	int tmp = 0;

	for (int PE = 0; PE < R*E; PE++)
	{
		MAC_ORDER = 1;

		for (int curr_cycle = 0; curr_cycle < standard_DDDG_cycle; curr_cycle++)
		{
			for (int id = 0; id < NODE_NUM; id++)
			{
				int PE_NUMBER = PE;

				if (*node[id].microop == NULL)
					continue;

				if (!strcmp(node[id].microop, "Mul"))
				{
					if (PE_NUMBER == node[id].pe_info)
					{
						if (curr_cycle == node[id].complete_cycle)
						{
							node[id].mac_order = MAC_ORDER;
							
							int child_id = node[id].childid[0];

							node[child_id].pe_info = PE;
							node[child_id].mac_order = MAC_ORDER;

							int rr = node[id].pe_info / E;
							int	ee = node[id].pe_info % E;

							pe_mac_order_id[rr][ee][MAC_ORDER] = id;

							MAC_ORDER++;

						}
					}
				}

				else
				{
					continue;
				}

			}
		}
	}
}