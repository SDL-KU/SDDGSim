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

#define REG_10ns_int_power                            5.535310e-04
#define REG_10ns_switch_power                         0.000000e+00
#define REG_10ns_dynamic_power                        5.535310e-04
#define REG_10ns_dynamic_energy                       5.535310e-03
#define REG_10ns_leakage_power                        7.358945e-05
#define REG_10ns_area                                 5.981433e+00

#define ADD_10ns_int_power                            4.832257e-03
#define ADD_10ns_switch_power                         3.701820e-03
#define ADD_10ns_dynamic_power                        8.534078e-03
#define ADD_10ns_dynamic_energy                       8.534078e-02
#define ADD_10ns_leakage_power                        2.380803e-03
#define ADD_10ns_area                                 1.794430e+02

#define MUL_10ns_int_power                            3.459049e-01
#define MUL_10ns_switch_power                         5.337095e-01
#define MUL_10ns_dynamic_power                        8.796144e-01
#define MUL_10ns_dynamic_energy                       8.796144e+00
#define MUL_10ns_leakage_power                        4.817683e-02
#define MUL_10ns_area                                 4.595000e+03

#define ALEX_NET_R 3
#define ALEX_NET_E 13	
const float part_read_energy[ALEX_NET_R + ALEX_NET_E] = { 0, 12.0219e-09, 12.0773e-09, 9.29636e-09, 7.95046e-09, 5.3648e-09, 4.8281e-09, 4.44925e-09, 4.14883e-09, 3.98592e-09, 3.80611e-09, 3.65939e-09, 3.52914e-09, 3.41567e-09, 2.39052e-09, 2.31018e-09 };
const float part_write_energy[ALEX_NET_R + ALEX_NET_E] = { 0, 9.43817e-09, 7.01547e-09, 5.88017e-09, 5.36676e-09, 4.50802e-09, 4.10245e-09, 3.81617e-09, 3.58904e-09, 3.48014e-09, 3.34275e-09, 3.23074e-09, 3.13135e-09, 3.04488e-09, 2.91669e-09, 2.7974e-09 };
const float part_leak_power[ALEX_NET_R + ALEX_NET_E] = { 0, 2.5838, 1.25895, 0.845507, 0.637265, 0.533745, 0.447229, 0.386159, 0.337696, 0.299861, 0.271716, 0.248686, 0.228213, 0.210296, 0.20786, 0.194318 };
const float part_area[ALEX_NET_R + ALEX_NET_E] = { 0, 169009, 74538.7, 51348.5, 39662.7, 31225.7, 26425.7, 23037.5, 20355.1, 18288.1, 16735.1, 15455.5, 14326.1, 13328.8, 11857.6, 11132.3 };
const float part_read_energy_output[ALEX_NET_E + 1] = { 0, 20.2251e-09, 15.1e-09, 11.6065e-09, 9.91323e-09, 6.67674e-09, 6.00085e-09, 5.52374e-09, 5.14603e-09, 4.94112e-09, 4.71828e-09, 4.53436e-09, 4.3693e-09, 4.22464e-09 };
const float part_write_energy_output[ALEX_NET_E + 1] = { 0, 13.8545e-09, 8.7294e-09, 7.30588e-09, 6.65969e-09, 5.60993e-09, 5.09899e-09,4.73832e-09, 4.4528e-09, 4.31581e-09, 4.14633e-09, 4.00608e-09, 3.87983e-09, 3.76913e-09 };
const float part_leak_power_output[ALEX_NET_E + 1] = { 0, 2.9044, 1.43453, 0.965555, 0.729334, 0.624053, 0.523291, 0.452164, 0.395855, 0.352332, 0.319732, 0.292932, 0.269077 };
const float part_area_output[ALEX_NET_E + 1] = { 0, 323508, 149922, 103939, 80786.4, 63127.3, 53551.1, 46791.5, 41440.1, 37333.1, 34234.9, 31664.1, 29410.8, 27439.1 };