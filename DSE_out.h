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

typedef struct _DDDG_info
{
	int DSE_cycle;
	float avg_fu_power;
	float avg_fu_dynamic_power;
	float fu_leakage_power;
	float avg_mem_power;
	float avg_mem_dynamic_power;
	float avg_mem_leakage_power;
	float avg_power;
	float fu_area;
	float mem_area;
	float total_area;
}DDDG_info;

void DSE_out(nodeInfo *node, int pe_mac_order[R][E][p*q*S*F+1], int last_node_id, input_memory *i_mem, weight_memory *w_mem, output_memory *o_ldr_mem, output_memory *o_str_mem, int i_pixel_id[H*W*C], int w_pixel_id[R*S*C*M], int o_load_pixel_id[E*F*M], int standard_dddg_cycle)
{
	ofstream DSE_OUTPUT;
	ofstream DSE_EXCEL;

	DDDG_info dddg_info[I_LATENCY * W_LATENCY * O_LATENCY * NUM_INPUT_BANK * NUM_WEIGHT_BANK * NUM_OUTPUT_BANK];
	int dddg_index = 0;

	char tmp_char1[200];
	char tmp_char2[200];

	int max_cycle = 0;
	int min_cycle = MAX_CYCLE;

	float max_power = 0;
	float min_power = 999;

	float max_area = 0;
	float min_area = 999999999999;

	int max_cycle_latency_info[3] = { 0, };
	int max_cycle_bank_info[3] = { 0, };

	int min_cycle_latency_info[3] = { 0, };
	int min_cycle_bank_info[3] = { 0, };

	int max_power_latency_info[3] = { 0, };
	int max_power_bank_info[3] = { 0, };

	int min_power_latency_info[3] = { 0, };
	int min_power_bank_info[3] = { 0, };

	int max_area_latency_info[3] = { 0, };
	int max_area_bank_info[3] = { 0, };

	int min_area_latency_info[3] = { 0, };
	int min_area_bank_info[3] = { 0, };

	int DSE_cycle;
	float avg_fu_power = 0;
	float avg_fu_dynamic_power = 0;
	float fu_leakage_power = 0;
	float avg_mem_power = 0;
	float avg_mem_dynamic_power = 0;
	float avg_mem_leakage_power = 0;
	float avg_power = 0;
	float fu_area = 0;
	float mem_area = 0;
	float total_area = 0;
	float max_power_info[3] = { 0.0, };
	float min_power_info[3] = { 0.0, };
	float reg_power = 0;
	float add_mul_power = 0;
	float memload_power = 0;
	float memstore_power = 0;

	ofstream power_break;
	char tmpChar[200];
	sprintf(tmpChar, "PowerBreak.txt");
	power_break.open(tmpChar);

	sprintf(tmp_char1, "DSE_OUTPUT_L[%d_%d_%d]_P[%d_%d_%d].txt", I_LATENCY, W_LATENCY, O_LATENCY, NUM_INPUT_BANK, NUM_WEIGHT_BANK, NUM_OUTPUT_BANK);
	sprintf(tmp_char2, "DSE_EXCEL_L[%d_%d_%d]_P[%d_%d_%d].txt", I_LATENCY, W_LATENCY, O_LATENCY, NUM_INPUT_BANK, NUM_WEIGHT_BANK, NUM_OUTPUT_BANK);

	DSE_OUTPUT.open(tmp_char1);
	DSE_EXCEL.open(tmp_char2);

	for (int i_latency = 1; i_latency <= 1; i_latency++)
	{
		for (int w_latency = 1; w_latency <= 1; w_latency++)
		{
			for (int o_latency = 1; o_latency <= 1; o_latency++)
			{
				for (int i_bank = 1; i_bank <= NUM_INPUT_BANK; i_bank++) // R+E - 1
				{
					for (int w_bank = 1; w_bank <= NUM_WEIGHT_BANK; w_bank++) // R
					{
						for (int o_bank = 1; o_bank <= NUM_OUTPUT_BANK; o_bank++) //E

						{
							if ((i_bank == 1) && (w_bank == 1) && (o_bank == 1) && (i_latency == 1) && (w_latency == 1) && (o_latency == 1))
							{
								DSE_cycle = standard_dddg_cycle;
							}
							else
							{
								DSE_cycle = mem_latency_bank_constraint(node, pe_mac_order, last_node_id, i_latency, w_latency, o_latency, i_bank, w_bank, o_bank, i_mem, w_mem, o_ldr_mem, o_str_mem, i_pixel_id, w_pixel_id, o_load_pixel_id);
							}

							//
							if ((i_bank == 1) && (w_bank == 2) && (o_bank == 1) && (i_latency == 3) && (w_latency == 3) && (o_latency == 3))
							{
								dump_graph(node, DSE_cycle);
								//int Xin;
								//cin >> Xin;
							}

							if ((i_bank == 1) && (w_bank == 1) && (o_bank == 2) && (i_latency == 3) && (w_latency == 3) && (o_latency == 3))
							{
								dump_graph(node, DSE_cycle);
								//int Xin;
								//cin >> Xin;
							}

							if ((i_bank == 2) && (w_bank == 1) && (o_bank == 1) && (i_latency == 3) && (w_latency == 3) && (o_latency == 3))
							{
								dump_graph(node, DSE_cycle);
								//int Xin;
								//cin >> Xin;
							}

							power_calc(node, DSE_cycle, &avg_fu_power, &avg_fu_dynamic_power, &fu_leakage_power, &avg_mem_power, &avg_mem_dynamic_power, &avg_mem_leakage_power, &avg_power,i_bank, w_bank, o_bank, &reg_power, &add_mul_power, &memload_power, &memstore_power);
							area_calc(node, DSE_cycle, &fu_area, &mem_area, &total_area, i_bank, w_bank, o_bank);

;							dddg_info[dddg_index].DSE_cycle = DSE_cycle;
							dddg_info[dddg_index].avg_fu_power = avg_fu_power;
							dddg_info[dddg_index].avg_fu_dynamic_power = avg_fu_dynamic_power;
							dddg_info[dddg_index].fu_leakage_power = fu_leakage_power;
							dddg_info[dddg_index].avg_mem_power = avg_mem_power;
							dddg_info[dddg_index].avg_mem_dynamic_power = avg_mem_dynamic_power;
							dddg_info[dddg_index].avg_mem_leakage_power = avg_mem_leakage_power;
							dddg_info[dddg_index].avg_power = avg_power;
							dddg_info[dddg_index].fu_area = fu_area;
							dddg_info[dddg_index].mem_area = mem_area;
							dddg_info[dddg_index].total_area = total_area;
							if (DSE_cycle > max_cycle)
							{
								max_cycle = DSE_cycle;
								max_cycle_latency_info[0] = i_latency;
								max_cycle_latency_info[1] = w_latency;
								max_cycle_latency_info[2] = o_latency;
								max_cycle_bank_info[0] = i_bank;
								max_cycle_bank_info[1] = w_bank;
								max_cycle_bank_info[2] = o_bank;
							}

							if (DSE_cycle < min_cycle)
							{
								min_cycle = DSE_cycle;
								min_cycle_latency_info[0] = i_latency;
								min_cycle_latency_info[1] = w_latency;
								min_cycle_latency_info[2] = o_latency;
								min_cycle_bank_info[0] = i_bank;
								min_cycle_bank_info[1] = w_bank;
								min_cycle_bank_info[2] = o_bank;
							}

							if (avg_power > max_power)
							{
								max_power = avg_power;
								max_power_latency_info[0] = i_latency;
								max_power_latency_info[1] = w_latency;
								max_power_latency_info[2] = o_latency;
								max_power_bank_info[0] = i_bank;
								max_power_bank_info[1] = w_bank;
								max_power_bank_info[2] = o_bank;
							}

							if (avg_power < min_power)
							{
								min_power = avg_power;
								min_power_latency_info[0] = i_latency;
								min_power_latency_info[1] = w_latency;
								min_power_latency_info[2] = o_latency;
								min_power_bank_info[0] = i_bank;
								min_power_bank_info[1] = w_bank;
								min_power_bank_info[2] = o_bank;
							}

							if (total_area > max_area)
							{
								max_area = total_area;
								max_area_latency_info[0] = i_latency;
								max_area_latency_info[1] = w_latency;
								max_area_latency_info[2] = o_latency;
								max_area_bank_info[0] = i_bank;
								max_area_bank_info[1] = w_bank;
								max_area_bank_info[2] = o_bank;
							}

							if (total_area < min_area)
							{
								min_area = total_area;
								min_area_latency_info[0] = i_latency;
								min_area_latency_info[1] = w_latency;
								min_area_latency_info[2] = o_latency;
								min_area_bank_info[0] = i_bank;
								min_area_bank_info[1] = w_bank;
								min_area_bank_info[2] = o_bank;
							}




							cout << "IP: " << i_bank << " ,WP: " << w_bank << " ,OP: " << o_bank << endl;
							cout << "IL: " << i_latency << " ,WL: " << w_latency << " ,OL: " << o_latency << endl;
							cout << "CYCLES: " << DSE_cycle << endl;
							cout << "AVG_FU_POWER: " << avg_fu_power << "mW " << endl;
							cout << "AVG_FU_DYNAMIC_POWER: " << avg_fu_dynamic_power << "mW " << endl;
							cout << "FU_LEAKAGE_POWER: "<< fu_leakage_power << "mW" << endl;
							cout << "AVG_MEM_POWER: " << avg_mem_power << "mW" << endl;
							cout << "AVG_MEM_DYNAMIC_POWER: " << avg_mem_dynamic_power << "mW" << endl;
							cout << "AVG_MEM_LEAKAGE_POWER: " << avg_mem_leakage_power << "mW" << endl;
							cout << "AVG_POWER: " << avg_power << "mW" << endl;
							cout << "FU_AREA: " << fu_area << "uM^2" << endl;
							cout << "MEM_AREA: " << mem_area << "uM^2" << endl;
							cout << "TOTAL_AREA: " << total_area << "uM^2" << endl;

							power_break << "reg_power = " << reg_power << endl;
							power_break << "add_mul_power = " << add_mul_power << endl;
							power_break << "memory_load_power = " << memload_power << endl;
							power_break << "memory_store_power = " << memstore_power << endl;


							DSE_OUTPUT << "IP: " << i_bank << " ,WP: " << w_bank << " ,OP: " << o_bank << endl;
							DSE_OUTPUT << "IL: " << i_latency << " ,WL: " << w_latency << " ,OL: " << o_latency << endl;
							DSE_OUTPUT << "CYCLES: " << DSE_cycle << endl;
							DSE_OUTPUT << "AVG_FU_POWER: " << avg_fu_power << "mW " << endl;
							DSE_OUTPUT << "AVG_FU_DYNAMIC_POWER: " << avg_fu_dynamic_power << "mW " << endl;
							DSE_OUTPUT << "FU_LEAKAGE_POWER: " << fu_leakage_power << "mW" << endl;
							DSE_OUTPUT << "AVG_MEM_POWER: " << avg_mem_power << "mW" << endl;
							DSE_OUTPUT << "AVG_MEM_DYNAMIC_POWER: " << avg_mem_dynamic_power << "mW" << endl;
							DSE_OUTPUT << "AVG_MEM_LEAKAGE_POWER: " << avg_mem_leakage_power << "mW" << endl;
							DSE_OUTPUT << "AVG_POWER: " << avg_power << "mW" << endl;
							DSE_OUTPUT << "FU_AREA: " << fu_area << "uM^2" << endl;
							DSE_OUTPUT << "MEM_AREA: " << mem_area << "uM^2" << endl;
							DSE_OUTPUT << "TOTAL_AREA: " << total_area << "uM^2" << endl;
							DSE_OUTPUT << "=============================" << endl;
							DSE_OUTPUT << endl;

							DSE_EXCEL << "[ " << i_latency << "  " << w_latency << "  " << o_latency << " ]" << "   " << "[ " << i_bank << "  " << w_bank << "  " << o_bank << " ]" << "   " << DSE_cycle << "	" << avg_fu_power << "	" << avg_fu_dynamic_power << "	" << fu_leakage_power <<"	"<<avg_mem_power<<"	"<<avg_mem_dynamic_power<<"	"<<avg_mem_leakage_power<<"	"<<avg_power<<"	"<<fu_area<<"	"<<mem_area<<"	"<<total_area<<endl;
							dddg_index++;
						}
					}
				}
			}
		}
	}
	
	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MAX CYCLE: " << max_cycle << endl;
	DSE_OUTPUT << "IP: " << max_cycle_bank_info[0] << " ,WP: " << max_cycle_bank_info[1] << " ,OP: " << max_cycle_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << max_cycle_latency_info[0] << " ,WL: " << max_cycle_latency_info[1] << " ,OL: " << max_cycle_latency_info[2] << endl;

	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MIN CYCLE: " << min_cycle << endl;
	DSE_OUTPUT << "IP: " << min_cycle_bank_info[0] << " ,WP: " << min_cycle_bank_info[1] << " ,OP: " << min_cycle_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << min_cycle_latency_info[0] << " ,WL: " << min_cycle_latency_info[1] << " ,OL: " << min_cycle_latency_info[2] << endl;

	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MAX POWER: " << max_power << endl;
	DSE_OUTPUT << "IP: " << max_power_bank_info[0] << " ,WP: " << max_power_bank_info[1] << " ,OP: " << max_power_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << max_power_latency_info[0] << " ,WL: " << max_power_latency_info[1] << " ,OL: " << max_power_latency_info[2] << endl;

	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MIN POWER: " << min_power << endl;
	DSE_OUTPUT << "IP: " << min_power_bank_info[0] << " ,WP: " << min_power_bank_info[1] << " ,OP: " << min_power_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << min_power_latency_info[0] << " ,WL: " << min_power_latency_info[1] << " ,OL: " << min_power_latency_info[2] << endl;

	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MAX AREA: " << max_area << endl;
	DSE_OUTPUT << "IP: " << max_area_bank_info[0] << " ,WP: " << max_area_bank_info[1] << " ,OP: " << max_area_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << max_area_latency_info[0] << " ,WL: " << max_area_latency_info[1] << " ,OL: " << max_area_latency_info[2] << endl;

	DSE_OUTPUT << endl;
	DSE_OUTPUT << "MIN AREA: " << min_area << endl;
	DSE_OUTPUT << "IP: " << min_area_bank_info[0] << " ,WP: " << min_area_bank_info[1] << " ,OP: " << min_area_bank_info[2] << endl;
	DSE_OUTPUT << "IL: " << min_area_latency_info[0] << " ,WL: " << min_area_latency_info[1] << " ,OL: " << min_area_latency_info[2] << endl;

	DSE_OUTPUT.close();
	DSE_EXCEL.close();
	power_break.close();
}