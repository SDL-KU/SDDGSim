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


typedef struct _output_memory {
	int order[E*F*M] = {0, };
	int valid = 0;
} output_memory;

typedef struct _weight_memory {
	int order[R*S*C*M] = { 0, };
	int valid = 0;
}weight_memory;

typedef struct _input_memory {
	int order[H*W*C] = { 0, };
	int valid = 0;
}input_memory;

//global variable:: node information
typedef struct _nodeInfo {

	char microop[10];
	int start_cycle;
	int complete_cycle;
	char label[10];
	int pixel;
	//long long address;
	int childid[NOP];
	int parentid[NOP];
	//int child_num;
	//int parent_num;
	int row_pe[R];
	int col_pe[E];
	bool used;
	bool output_used;
	bool time_dependence;

	//
	//int parent_dependence[10] = { 0, };
	//int child_dependence[10] = { 0, };

	int ll_child_dependence = -1;
	int ll_parent_dependence = -1;

	//mac_info
	int   pe_info = 999;
	int   mac_order = 0;
	int   late_cnt = 0;
	bool  pe_info_set = FALSE;
	bool  already_load = FALSE;
}nodeInfo;

//C = 4, q = 4, M = 1, p = 1
//int inputorder[H * W * C] = { 0,3,6,9,12,15,18,21,24,27,30,33,1,4,7,10,13,16,19,22,25,28,31,34,2,5,8,11,14,17,20,23,26,29,32,35 };
//int weightorder[R * S * C * M] = { 0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15 };
//int outputorder[E * F * M] = { 0,2,1,3 };
//int str_outputorder[E*F*M] = { 0,2,1,3 };

// M = 4, p = 4, C = 1, q = 1
//int inputorder[H * W * C ] = {0,3,6,1,4,7,2,5,8};
//int weightorder[R * S * C * M]    = { 0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15 };
//int outputorder[E * F * M]        = { 0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15 };
//int str_outputorder[E * F * M]    = { 0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15 };

// M = 1, C = 1, 
//int inputorder[H * W * C * (M/p)] = {0,3,6,1,4,7,2,5,8};
//int weightorder[R * S * C * M] = { 0,2,1,3};
//int outputorder[E * F * M] = { 0,2,1,3};
//int str_outputorder[E * F * M] = { 0,2,1,3};

//3by3 M1 C2 q2
//int inputorder[H * W * C * (M/p)] = {0,5,10,15,20, 25,30,35,40,45, 1,6,11,16,21, 26,31,36,41,46 ,2,7,12,17,22, 27,32,37,42,47 ,3,8,13,18,23, 28,33,38,43,48, 4,9,14,19,24, 29,34,39,44,49};
//int weightorder[R * S * C * M] = { 0,1,2, 9,10,11, 3,4,5, 12,13,14, 6,7,8, 15,16,17};
//int outputorder[E * F * M] = { 0,3,6, 1,4,7 ,2,5,8 };
//int str_outputorder[E * F * M] = { 0,3,6, 1,4,7 ,2,5,8 };

//
//int inputorder[H * W * C * (M / p)] = { 0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69
//									   ,1,4,7,10,13,16,19,22,25,28,31,34,37,40,43,46,49,52,55,58,61,64,67,70
//									   ,2,5,8,11,14,17,20,23,26,29,32,35,38,41,44,47,50,53,56,59,62,65,68,71};
//
//int weightorder[R * S * C * M] = { 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,
//                                   1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31};
//
//int outputorder[E * F * M] = { 0,2,1,3 };
//int str_outputorder[E * F * M] = { 0,2,1,3 };

// name
char input_name[WORD] = { "ifmap" };
char weight_name[WORD] = { "fmap" };
char output_name[WORD] = { "ofmap" };