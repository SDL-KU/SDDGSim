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

void print_dddg(nodeInfo *node, int cycle)
{
	ofstream cycle_output;
	ofstream cycle_output11;

	int tmp1 = 0;
	int tmp2 = 0;

	
	cycle_output.open("cycle_output.txt");
	cycle_output11.open("cycle_output11.txt");

	for (int cc = 0; cc < cycle; cc++)
	{
		cycle_output << "CYCLE = " << cc << endl;
		cycle_output << "=========================" << endl;

		for (int id = 0; id < NODE_NUM + NODE_NUM_LOC; id++)
		{
			if ((*node[id].microop == NULL))
				continue;

			if (cc == node[id].complete_cycle)
			{
				//cout << "CYCLE = " << cc << endl;
				cycle_output << "NODE ID = " << id << endl;
				cycle_output << "NODE MICRO OP = " << node[id].microop << endl;
				if (!strcmp(node[id].microop, "Load") || !strcmp(node[id].microop, "Store"))
				{
					cycle_output << "NODE LABEL = " << node[id].label << endl;
					cycle_output << "NODE PIXEL = " << node[id].pixel << endl;
				}

				if (!strcmp(node[id].microop, "Mul") || !strcmp(node[id].microop, "Add") || !strcmp(node[id].microop, "LOC_ADD"))
				{
					if (!strcmp(node[id].microop, "LOC_ADD"))
					{
						cycle_output <<"**pixel :" << node[id].pixel << endl;
						cycle_output << "**PE =  " << node[id].pe_info << endl;
					}
					else
					{
						for (int ii = 0; ii < NOP; ii++)
						{
							int KK = 0;
							KK = node[id].parentid[ii];

							if (KK > 0)
							{
								cycle_output << node[KK].label << " pixel :" << node[KK].pixel << endl;
							}
						}
						cycle_output << "**PE =  " << node[id].pe_info << endl;
						cycle_output << "**MAC ORDER =  " << node[id].mac_order << endl;
					}
				}
				cycle_output << "---------------------------" << endl;
			}
		}
		cycle_output << "=========================" << endl;
	}
	cycle_output << tmp1 << endl;
	cycle_output << tmp2 << endl;
	cycle_output.close();
}