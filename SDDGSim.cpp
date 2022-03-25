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

#define _CRT_SECURE_NO_WARNINGS
//#pragma comment(linker, "/HEAP:4000000000")
//#pragma comment(linker, "/STACK:4000000000")
using namespace std;

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <fstream>
#include "parameters.h"
#include "global_variables.h"
#include "dump_txt.h"
#include "search_node.h"
#include "PE.h"
#include "FIFO.h"
#include "init_mac_str.h"
#include "initialize_mem_pe.h"
#include "print_dddg.h"
#include "rs_dataflow_scheduling.h"
#include "find_pixel_id.h"
#include "pe_mac_order.h"
#include "dependence_check.h"
#include "find_max_cycle.h"
#include "additional_constraint.h"
#include "dump_excel.h"
#include "power_library.h"
#include "power_function.h"
#include "DSE_out.h"


int main()
{
	int aladdin_cycle = 0;
	int standard_dddg_cycle = 0;
	int last_node_id = 0;
	int pe_mac_order_id[R][E][p * q * S * F +1] = { 0, };
	int i_pixel_id[H*W*C];
	int w_pixel_id[R*S*C*M];
	int o_load_pixel_id[E*F*M];

	// node information
	nodeInfo *node = (nodeInfo*)calloc(NODE_NUM + NODE_NUM_LOC, sizeof(nodeInfo));

	// GLB
	input_memory  i_mem;
	weight_memory w_mem;
	output_memory o_ldr_mem;
	output_memory o_str_mem;

	// PE 
	PE aladdin_pe[R][E];

	// Initialize nodes
	load_text(node,&last_node_id);
	mac_store_initialize(node);
	
	// Initialize memory & PE
	initialze_mem_order(&i_mem, &w_mem, &o_ldr_mem, &o_str_mem);
	initialize_PE(aladdin_pe);

	//aladdin scheduling for RS
	rs_dataflow_w_fifo(node, aladdin_pe, &i_mem, &w_mem, &o_ldr_mem, &o_str_mem, last_node_id);
	reschedule_str_node(node);
	find_max_cycle(node, &standard_dddg_cycle);
	set_node_to_pe_dependence(node, pe_mac_order_id, standard_dddg_cycle);

	//dependence
	local_link_dependence_update(node);

	//constraint
	find_load_pixel_id(node, i_pixel_id, w_pixel_id, o_load_pixel_id);
	DSE_out(node, pe_mac_order_id, last_node_id, &i_mem, &w_mem, &o_ldr_mem, &o_str_mem, i_pixel_id, w_pixel_id, o_load_pixel_id, standard_dddg_cycle);

	return 0;
}