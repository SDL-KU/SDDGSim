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


#define NODE_NUM  815000
#define NODE_NUM_LOC (p * (R-1) * E * F)
#define WORD      100
#define MAX_CYCLE 70000
#define NON_INITIAL -55555


// CNN Parameter
#define N 1
#define M 16 //output channel
#define C 4 //filter channel
#define H 15 //input column
#define W 15 //input row
#define R 3  //weight column
#define S 3  //weight row
#define U 1
#define E ((H-R)/U+1) //output column
#define F ((W-S)/U+1) //output row

// RS dataflow
#define p M
#define q C

//
#define NOP 3
#define TRUE 1
#define FALSE 0
#define UNINITIALIZED -1
#define INITIALIZE -1
#define BLANK -999
#define EOFF -100

// Constraint: Latency
#define I_LATENCY 1//4 
#define W_LATENCY 1//4
#define O_LATENCY 1//4

// Constraint: # Ports
#define NUM_INPUT_BANK  3 //(R+E-1)
#define NUM_WEIGHT_BANK 3 // R
#define NUM_OUTPUT_BANK 3 // E
