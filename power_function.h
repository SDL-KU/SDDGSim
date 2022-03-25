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

void power_calc(nodeInfo* node, int num_cycles, float* avg_fu_power, float* avg_fu_dynamic_power, float* fu_leakage_power, float* avg_mem_power, float* avg_mem_dynamic_power, float* avg_mem_leakage_power, float* avg_power, int input_port, int weight_port, int output_port, float *reg_power, float *add_mul_power, float *memory_load_power, float *memory_store_power)
{
	float cycleTime = 10e-09;
	int total_input_loads_to_glb = H * W*C;
	int total_weight_loads_to_glb = R * S*M*C;
	int total_output_loads_to_glb = E * F*M;

	int total_mul = R * E*S*C*F*M; //total mac for all pe 
	int total_add = R * E*(S*C*F*M + F * M); //add+local link add+output loads
	int total_input_loads_to_pe = R * E*(W*C); //total loads for all pe
	int total_weight_loads_to_pe = R * E*(S*C*M);
	int total_output_loads_to_pe = E * (F*M);
	int total_stores = E * (F*M); // total stores for all pe
	int total_reg_reads = R * E*(3 * S*C + 1)*F*M;
	int total_reg_writes = R * E*(S*C + 1)*F*M + total_input_loads_to_pe + total_weight_loads_to_pe;


	//FU DYNAMIC
	float mul_dynamic_power = (MUL_10ns_switch_power + MUL_10ns_int_power) * total_mul;
	float add_dynamic_power = (ADD_10ns_switch_power + ADD_10ns_int_power) * total_add;
	float fu_dynamic_energy = (mul_dynamic_power + add_dynamic_power) * cycleTime;
	float reg_dynamic_energy = (REG_10ns_int_power + REG_10ns_switch_power) * (total_reg_reads + total_reg_writes) * 32 * cycleTime;
	float reg_dynamic_power = reg_dynamic_energy;
	fu_dynamic_energy += reg_dynamic_energy;

	//FU LEAKAGE
	int max_add = R * E;
	int max_mul = R * E;
	int max_reg = R * E*(p*q*S + q * S + p); // spadÀÇ ÃÑÇÕ
	float add_leakage_power = ADD_10ns_leakage_power * max_add;
	float mul_leakage_power = MUL_10ns_leakage_power * max_mul;
	float reg_leakage_power = REG_10ns_leakage_power * 32 * max_reg;
	*fu_leakage_power = mul_leakage_power + add_leakage_power + reg_leakage_power;
	*avg_fu_dynamic_power = fu_dynamic_energy / (num_cycles*cycleTime);
	*avg_fu_power = *avg_fu_dynamic_power + *fu_leakage_power;

	//MEM DYNAMIC & LEAKAGE
	float load_energy = total_input_loads_to_glb * part_read_energy[input_port] + total_weight_loads_to_glb * part_read_energy[weight_port] + total_output_loads_to_glb * part_read_energy_output[output_port];
	float store_energy = total_stores * part_write_energy_output[output_port];

	*avg_mem_dynamic_power = (load_energy + store_energy) / (num_cycles*cycleTime);
	*avg_mem_leakage_power = part_leak_power[input_port] * input_port + part_leak_power[weight_port] * weight_port + part_leak_power_output[output_port] * output_port;
	*avg_mem_power = *avg_mem_dynamic_power + *avg_mem_leakage_power;
	*avg_power = *avg_fu_power + *avg_mem_power;

	*reg_power = (reg_dynamic_energy) / (num_cycles*cycleTime) + reg_leakage_power;
	*add_mul_power = ((mul_dynamic_power + add_dynamic_power) * cycleTime) / (num_cycles*cycleTime) + mul_leakage_power + add_leakage_power;
	*memory_load_power = load_energy / (num_cycles*cycleTime);
	*memory_store_power = store_energy / (num_cycles*cycleTime);
}

void area_calc(nodeInfo* node, int num_cycles, float *fu_area, float *mem_area, float *total_area, int input_port, int weight_port, int output_port)
{
	int max_add = R * E; //PEÀÇ °¹¼ö¿Í °°´Ù.
	int max_mul = R * E;
	int max_reg = R * E * (p*q*S + q * S + p);

	*fu_area = ADD_10ns_area * max_add + MUL_10ns_area * max_mul + REG_10ns_area * 32 * max_reg;
	*mem_area = part_area[input_port] * input_port + part_area[weight_port] * weight_port + part_area_output[output_port] * output_port;
	*total_area = *fu_area + *mem_area;
}