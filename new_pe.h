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

typedef struct _pe1 {

	int spad_ifmap[q*S];
	int spad_fmap[q*p*S];
	int spad_ofmap[p];

	int i_r_ptr;
	int i_t_ptr;
	int i_t_ptr_offset;
	int i_r_ptr_cnt;

	int f_r_ptr;
	int f_t_ptr;
	int f_r_ptr_cnt;

	int o_r_ptr;
	int o_t_ptr;
	int o_r_ptr_cnt;


	bool spad_ifmap_full_flag;
	bool spad_fmap_full_flag;


	bool ofmap_gon_valid_curr;
	bool ofmap_gon_valid_past;

	bool ifmap_ready_next;
	bool ifmap_ready_curr;

	int ifmap_q_cnt;
	int ifmap_offset_cnt;



	bool fmap_ready;

	bool ofmap_gin_ready_next;
	bool ofmap_gin_ready_curr;

	bool ifmap_handshake_curr;
	bool ifmap_handshake_past;

	int ifmap_handshake_cnt;
	int fmap_handshake_cnt;

	bool fmap_handshake_curr;
	bool fmap_handshake_past;

	bool ofmap_gin_handshake_curr;
	bool ofmap_gin_handshake_past;

	int ofmap_gin_handshake_cnt;
	int ofmap_gin_handshake_cnt_temp;

	bool ofmap_gon_handshake_curr;
	bool ofmap_gon_handshake_past;

	int ofmap_gon_handshake_cnt;

	bool mult_enable_curr;
	bool mult_enable_past;

	int mult_cnt;
	int mult_cnt_ff;
	int mult_cnt_temp;

	int mult_value;

	bool mult_mux_sel;

	bool psum_mux_sel;

	int psum_mux_sel_cnt;

	int mult_mux_value;
	int psum_mux_value;

	bool add_enable_curr;
	bool add_enable_past;

	int add_value;
	int add_cnt;
	int add_cnt_ff;

	int dummy = 0;
}PE1;

void pe_main_action(PE1 pe[R][E]) {

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			pe[rr][ee].ofmap_gon_valid_past = pe[rr][ee].ofmap_gon_valid_curr;

			pe[rr][ee].ifmap_ready_curr = pe[rr][ee].ifmap_ready_next;
			pe[rr][ee].ofmap_gin_ready_curr = pe[rr][ee].ofmap_gin_ready_next;

			pe[rr][ee].ifmap_handshake_past = pe[rr][ee].ifmap_handshake_curr;
			pe[rr][ee].fmap_handshake_past = pe[rr][ee].fmap_handshake_curr;
			pe[rr][ee].ofmap_gin_handshake_past = pe[rr][ee].ofmap_gin_handshake_curr;
			pe[rr][ee].ofmap_gon_handshake_past = pe[rr][ee].ofmap_gon_handshake_curr;

			pe[rr][ee].mult_enable_past = pe[rr][ee].mult_enable_curr;
			pe[rr][ee].mult_cnt_ff = pe[rr][ee].mult_cnt;
			pe[rr][ee].add_enable_past = pe[rr][ee].add_enable_curr;
			pe[rr][ee].add_cnt_ff = pe[rr][ee].add_cnt;
		}
	}
}

void ifmap_spad_ctrl(PE1 pe[R][E], int rr, int ee, int ifmap_valid)
{

	//receive indicator
	if (pe[rr][ee].ifmap_handshake_past)
	{
		pe[rr][ee].i_r_ptr = (++pe[rr][ee].i_r_ptr_cnt) % (q*S);
	}

	if (pe[rr][ee].ifmap_ready_curr && ifmap_valid)
	{
		pe[rr][ee].ifmap_handshake_curr = 1;

		pe[rr][ee].ifmap_handshake_cnt++;

		if (pe[rr][ee].ifmap_handshake_cnt == q * S)
			pe[rr][ee].spad_ifmap_full_flag = 1;

		if (pe[rr][ee].spad_ifmap_full_flag) {
			pe[rr][ee].ifmap_q_cnt = pe[rr][ee].ifmap_handshake_cnt % q;
			if (pe[rr][ee].ifmap_q_cnt == 0)
				pe[rr][ee].ifmap_offset_cnt++;
		}
	}
	else
		pe[rr][ee].ifmap_handshake_curr = 0;


	//trans indicator
	if (pe[rr][ee].mult_enable_past) {
		if (pe[rr][ee].mult_cnt_ff%p == 0) {
			if (pe[rr][ee].mult_cnt_ff % (p*q*S) == 0 && pe[rr][ee].mult_cnt_ff != 0) {
				pe[rr][ee].i_t_ptr = ((++pe[rr][ee].i_t_ptr) + pe[rr][ee].i_t_ptr_offset) % (q*S);
			}
			else
				pe[rr][ee].i_t_ptr = (++pe[rr][ee].i_t_ptr) % (q*S);
		}
	}
}

void fmap_spad_ctrl(PE1 pe[R][E], int rr, int ee, int fmap_valid)
{
	///////////////////////////////             fmap spad ctrl

	//receive indicator
	if (pe[rr][ee].fmap_handshake_past)
	{
		pe[rr][ee].f_r_ptr = (++pe[rr][ee].f_r_ptr_cnt) % (p*q*S);
	}



	if (pe[rr][ee].fmap_ready && fmap_valid)
	{
		pe[rr][ee].fmap_handshake_curr = 1;
		pe[rr][ee].fmap_handshake_cnt++;


		if (pe[rr][ee].fmap_handshake_cnt == p * q*S)
			pe[rr][ee].spad_fmap_full_flag = 1;
	}
	else
		pe[rr][ee].fmap_handshake_curr = 0;



	//trans indicator
	if (pe[rr][ee].mult_enable_past) {
		pe[rr][ee].f_t_ptr = (++pe[rr][ee].f_t_ptr) % (p*q*S);
	}
}

void ofmap_spad_ctrl(PE1 pe[R][E], int rr, int ee, int ofmap_valid_gin)
{
	///////////////////////////////             ofmap spad ctrl
	//receive indicator
	if (pe[rr][ee].ofmap_gin_handshake_past)
	{
		pe[rr][ee].o_r_ptr = (++pe[rr][ee].o_r_ptr_cnt) % p;
	}


	if (pe[rr][ee].ofmap_gin_ready_curr && ofmap_valid_gin) {
		pe[rr][ee].ofmap_gin_handshake_curr = 1;

		pe[rr][ee].ofmap_gin_handshake_cnt++;
	}
	else
		pe[rr][ee].ofmap_gin_handshake_curr = 0;


	//trans indicator
	if (pe[rr][ee].mult_enable_past) {
		pe[rr][ee].o_t_ptr = (++pe[rr][ee].o_t_ptr) % p;
	}
}

void mult_enable_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             Mult enable signal
	if (pe[rr][ee].mult_cnt_ff < p*q*S*F)
	{
		if (pe[rr][ee].ofmap_gon_valid_past && !pe[rr][ee].ofmap_gon_handshake_past)
			pe[rr][ee].mult_enable_curr = 0;
		else {
			if (!pe[rr][ee].ofmap_gin_handshake_curr) {
				//if (   (((mult_cnt/(p*q*S) )-1)  *p)  +(mult_cnt+1) > add_cnt             ) {
				if (pe[rr][ee].mult_cnt_ff % (p*q*S) == 0 &&
					pe[rr][ee].add_cnt_ff % (p*q*S + p) >= p * q*S &&
					pe[rr][ee].add_cnt_ff % (p*q*S + p) <= p * q*S + p - 1) {

					pe[rr][ee].mult_enable_curr = 0;

				}
				else {
					//if (!spad_ifmap_full_flag || !spad_fmap_full_flag) {
					if (pe[rr][ee].i_r_ptr_cnt < q*S || pe[rr][ee].f_r_ptr_cnt < p*q*S) {
						if (pe[rr][ee].mult_cnt_ff < p*pe[rr][ee].i_r_ptr_cnt &&
							pe[rr][ee].mult_cnt_ff < pe[rr][ee].f_r_ptr_cnt) {
							pe[rr][ee].mult_enable_curr = 1;
						}
						else {
							pe[rr][ee].mult_enable_curr = 0;
						}
					}
					else {
						if (pe[rr][ee].i_r_ptr != pe[rr][ee].i_t_ptr) {
							pe[rr][ee].mult_enable_curr = 1;
						}
						else {
							if (pe[rr][ee].ifmap_ready_curr == 0) {
								pe[rr][ee].mult_enable_curr = 1;
							}
							else
								pe[rr][ee].mult_enable_curr = 0;
						}
					}
				}
			}
			else
				pe[rr][ee].mult_enable_curr = 0;
		}
	}
	else
		pe[rr][ee].mult_enable_curr = 0;

	if (pe[rr][ee].mult_enable_curr) {
		pe[rr][ee].mult_cnt++;
	}
}

void mult_mux_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             mult mux signal
	if (pe[rr][ee].mult_enable_curr)
		pe[rr][ee].mult_mux_sel = 0;
	else if (pe[rr][ee].ofmap_gin_handshake_curr)
		pe[rr][ee].mult_mux_sel = 1;
	else
		pe[rr][ee].mult_mux_sel = 0;
}

void add_enable_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             add enable signal
	//add
	if (pe[rr][ee].mult_enable_curr || pe[rr][ee].ofmap_gin_handshake_curr) {
		pe[rr][ee].add_enable_curr = 1;
		pe[rr][ee].add_cnt++;
	}
	else
		pe[rr][ee].add_enable_curr = 0;
}

void psum_mux_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             psum mux signal
	if (pe[rr][ee].psum_mux_sel_cnt < p) {
		if (pe[rr][ee].ofmap_gin_handshake_curr) {
			if (pe[rr][ee].ofmap_gin_handshake_cnt%p == 0) {
				pe[rr][ee].psum_mux_sel = 1;
			}
			else if (pe[rr][ee].mult_cnt % (p*q*S) < pe[rr][ee].ofmap_gin_handshake_cnt % p)
				pe[rr][ee].psum_mux_sel = 1;
			else
				pe[rr][ee].psum_mux_sel = 0;
		}
		else if (pe[rr][ee].mult_enable_curr) {
			if (pe[rr][ee].ofmap_gin_handshake_cnt%p == 0) {
				pe[rr][ee].psum_mux_sel = 1;
			}
			else if (pe[rr][ee].mult_cnt % (p*q*S) > pe[rr][ee].ofmap_gin_handshake_cnt % p)
				pe[rr][ee].psum_mux_sel = 1;
			else
				pe[rr][ee].psum_mux_sel = 0;
		}
		else
			pe[rr][ee].psum_mux_sel = 0;
	}
	else
		pe[rr][ee].psum_mux_sel = 0;

	if (pe[rr][ee].add_cnt % (p*q*S + p) == 0)
		pe[rr][ee].psum_mux_sel_cnt = 0;
	else if (pe[rr][ee].psum_mux_sel)
		pe[rr][ee].psum_mux_sel_cnt++;
}

void ofmap_gon_valid_signal(PE1 pe[R][E], int rr, int ee/*, bool ofmap_gon_valid*/)
{

	/////////////////////////////////    ofmap_gon_valid signal
	if (pe[rr][ee].mult_cnt != 0 && pe[rr][ee].mult_cnt % (p*q*S) == 0) {
		if ((pe[rr][ee].mult_cnt - 1) / (p*q*S) == (pe[rr][ee].ofmap_gin_handshake_cnt - 1) / p)
			pe[rr][ee].mult_cnt_temp = p * q*S;
		else
			pe[rr][ee].mult_cnt_temp = pe[rr][ee].mult_cnt % (p*q*S);
	}
	else
		pe[rr][ee].mult_cnt_temp = pe[rr][ee].mult_cnt % (p*q*S);

	if (pe[rr][ee].ofmap_gin_handshake_cnt != 0 && pe[rr][ee].ofmap_gin_handshake_cnt % (p) == 0) {
		if ((pe[rr][ee].mult_cnt - 1) / (p*q*S) == (pe[rr][ee].ofmap_gin_handshake_cnt - 1) / p)
			pe[rr][ee].ofmap_gin_handshake_cnt_temp = p;
		else
			pe[rr][ee].ofmap_gin_handshake_cnt_temp = pe[rr][ee].ofmap_gin_handshake_cnt % (p);
	}
	else
		pe[rr][ee].ofmap_gin_handshake_cnt_temp = pe[rr][ee].ofmap_gin_handshake_cnt % (p);

	if (pe[rr][ee].mult_enable_curr) {
		if (pe[rr][ee].mult_cnt_temp >= p * q*S - p + pe[rr][ee].o_t_ptr + 1 &&
			pe[rr][ee].ofmap_gin_handshake_cnt_temp >= pe[rr][ee].o_t_ptr + 1)
			pe[rr][ee].ofmap_gon_valid_curr = 1;
		else
			pe[rr][ee].ofmap_gon_valid_curr = 0;
	}
	else if (pe[rr][ee].ofmap_gin_handshake_curr) {
		if (pe[rr][ee].mult_cnt_temp >= p * q*S - p + pe[rr][ee].o_r_ptr + 1 &&
			pe[rr][ee].ofmap_gin_handshake_cnt_temp >= pe[rr][ee].o_r_ptr + 1)
			pe[rr][ee].ofmap_gon_valid_curr = 1;
		else
			pe[rr][ee].ofmap_gon_valid_curr = 0;
	}
	else
		pe[rr][ee].ofmap_gon_valid_curr = 0;

	//ofmap_gon_valid = pe[rr][ee].ofmap_gon_valid_curr;
}

void ofmap_handshake(PE1 pe[R][E], int rr, int ee, bool ofmap_ready_gon)
{
	///////////////////////////////             ofmap handshake

	//ofmap_gon_handshake
	if (pe[rr][ee].ofmap_gon_valid_curr && ofmap_ready_gon) {
		pe[rr][ee].ofmap_gon_handshake_curr = 1;
		pe[rr][ee].ofmap_gon_handshake_cnt++;
	}
	else
		pe[rr][ee].ofmap_gon_handshake_curr = 0;
}

void if_spad_to_fifo_ready_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             spad_to_fifo_ready signal

	//    ifmap_ready
	if (pe[rr][ee].spad_ifmap_full_flag)
	{
		if (pe[rr][ee].ifmap_handshake_cnt != W * q) {
			if (pe[rr][ee].mult_cnt >=
				(pe[rr][ee].ifmap_q_cnt*p + (pe[rr][ee].ifmap_offset_cnt - 1)*p*q*S + p))
				pe[rr][ee].ifmap_ready_next = 1;
			else
				pe[rr][ee].ifmap_ready_next = 0;
		}
		else
			pe[rr][ee].ifmap_ready_next = 0;
	}
	else {
		pe[rr][ee].ifmap_ready_next = 1;
	}
}

void f_spad_to_fifo_ready_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             spad_to_fifo_ready signal

	//    fmap_ready
	if (pe[rr][ee].spad_fmap_full_flag) {
		if (pe[rr][ee].add_cnt == F * (p*q*S + p) && pe[rr][ee].ofmap_gon_valid_curr)
			pe[rr][ee].fmap_ready = 1;
		else
			pe[rr][ee].fmap_ready = 0;
	}
	else
		pe[rr][ee].fmap_ready = 1;
}

void of_spad_to_fifo_ready_signal(PE1 pe[R][E], int rr, int ee)
{
	///////////////////////////////             spad_to_fifo_ready signal

	//    ofmap_gin_ready
	if (pe[rr][ee].add_cnt % (p*q*S + p) == 0)
		pe[rr][ee].ofmap_gin_ready_next = 1;
	else if (pe[rr][ee].o_r_ptr == p - 1 && pe[rr][ee].ofmap_gin_handshake_curr)
		pe[rr][ee].ofmap_gin_ready_next = 0;
}

void fifo_to_spad(PE1 pe[R][E], int rr, int ee, int input_id, int weight_id, int output_id)
{
	int tmp;

	if (pe[rr][ee].ifmap_handshake_curr)
		pe[rr][ee].spad_ifmap[pe[rr][ee].i_r_ptr] = input_id;

	if (pe[rr][ee].fmap_handshake_curr)
		pe[rr][ee].spad_fmap[pe[rr][ee].f_r_ptr] = weight_id;

	int temp;

	if (pe[rr][ee].ofmap_gin_handshake_curr)
		temp = pe[rr][ee].o_r_ptr;
	else
		temp = pe[rr][ee].o_t_ptr;

	int add_value = 0;
	int mult_mux_value = 0;
	int psum_mux_value = 0;
	int ofmap = 0;

	//add
	add_value = mult_mux_value + psum_mux_value;

	/*
	if (!pe[rr][ee].ofmap_gon_valid_curr) {
		if (pe[rr][ee].ofmap_gin_handshake_curr)
			pe[rr][ee].spad_ofmap[temp] = output_id;
		else
			pe[rr][ee].spad_ofmap[temp] = pe[rr][ee].spad_ofmap[temp];
	}
	else
		ofmap = add_value;
	*/
	
	if (!pe[0][ee].ofmap_gon_valid_curr) {
		if (pe[0][ee].ofmap_gin_handshake_curr)
			pe[0][ee].spad_ofmap[temp] = output_id;
		else
			pe[0][ee].spad_ofmap[temp] = pe[0][ee].spad_ofmap[temp];
	}
	else
		ofmap = add_value;
}

void pe_main_update(PE1 pe[R][E], int input_id, int weight_id, int output_id, int *ifmap_valid, int *fmap_valid, int *ofmap_valid)
{
	bool XX = 0;

	for (int rr = 0; rr < R; rr++)
	{
		for (int ee = 0; ee < E; ee++)
		{
			ifmap_spad_ctrl(pe, rr, ee, ifmap_valid[rr*E + ee]);

			fmap_spad_ctrl(pe, rr, ee, fmap_valid[rr*E + ee]);

			if (rr == 0)
			{
				ofmap_spad_ctrl(pe, rr, ee, ofmap_valid[rr*E + ee]);
			}
			else
			{
				ofmap_spad_ctrl(pe, rr, ee, pe[rr - 1][ee].ofmap_gon_valid_past);
			}

			mult_enable_signal(pe, rr, ee);

			mult_mux_signal(pe, rr, ee);

			add_enable_signal(pe, rr, ee);

			psum_mux_signal(pe, rr, ee);

//			if (rr == R - 1)
//			{
//				ofmap_gon_valid_signal(pe, rr, ee, XX);
//			}
//			else
//			{
//				ofmap_gon_valid_signal(pe, rr, ee, pe[rr][ee].ofmap_gon_valid_curr);
//			}

			ofmap_gon_valid_signal(pe, rr, ee);

			if (rr == R - 1)
			{
				ofmap_handshake(pe, rr, ee, !XX);
			}
			else
			{
				ofmap_handshake(pe, rr, ee, pe[rr+1][ee].ofmap_gin_ready_curr);
			}

			if_spad_to_fifo_ready_signal(pe, rr, ee);

			f_spad_to_fifo_ready_signal(pe, rr, ee);

			of_spad_to_fifo_ready_signal(pe, rr, ee);
		}
	}
}

void pe_member_var_initializer(PE1 pe[R][E], int rr, int ee)
{
	////////////      test
	pe[rr][ee].ofmap_gon_valid_curr = 0;
	pe[rr][ee].ofmap_gon_valid_past = 0;

	pe[rr][ee].ifmap_ready_next = 1;
	pe[rr][ee].ifmap_ready_curr = 1;

	pe[rr][ee].ifmap_q_cnt = 0;
	pe[rr][ee].ifmap_offset_cnt = 0;


	pe[rr][ee].fmap_ready = 1;

	pe[rr][ee].ofmap_gin_ready_next = 1;
	pe[rr][ee].ofmap_gin_ready_curr = 1;


	pe[rr][ee].i_r_ptr = 0;
	pe[rr][ee].i_t_ptr = 0;
	pe[rr][ee].i_t_ptr_offset = q;
	pe[rr][ee].i_r_ptr_cnt = 0;

	pe[rr][ee].f_r_ptr = 0;
	pe[rr][ee].f_t_ptr = 0;
	pe[rr][ee].f_r_ptr_cnt = 0;

	pe[rr][ee].o_r_ptr = 0;
	pe[rr][ee].o_t_ptr = 0;
	pe[rr][ee].o_r_ptr_cnt = 0;


	pe[rr][ee].spad_ifmap_full_flag = 0;
	pe[rr][ee].spad_fmap_full_flag = 0;


	pe[rr][ee].ifmap_handshake_curr = 0;
	pe[rr][ee].ifmap_handshake_past = 0;

	pe[rr][ee].ifmap_handshake_cnt = 0;
	pe[rr][ee].fmap_handshake_cnt = 0;

	pe[rr][ee].fmap_handshake_curr = 0;
	pe[rr][ee].fmap_handshake_past = 0;

	pe[rr][ee].ofmap_gin_handshake_curr = 0;
	pe[rr][ee].ofmap_gin_handshake_past = 0;

	pe[rr][ee].ofmap_gon_handshake_curr = 0;
	pe[rr][ee].ofmap_gon_handshake_past = 0;

	pe[rr][ee].ofmap_gin_handshake_cnt = 0;

	pe[rr][ee].ofmap_gin_handshake_cnt_temp = 0;

	pe[rr][ee].ofmap_gon_handshake_cnt = 0;

	pe[rr][ee].mult_enable_curr = 0;
	pe[rr][ee].mult_enable_past = 0;

	pe[rr][ee].mult_cnt = 0;
	pe[rr][ee].mult_value = 0;

	pe[rr][ee].mult_cnt_temp = 0;

	pe[rr][ee].mult_mux_sel = 0;
	pe[rr][ee].psum_mux_sel = 0;

	pe[rr][ee].psum_mux_sel_cnt = 0;

	pe[rr][ee].mult_mux_value = 0;
	pe[rr][ee].psum_mux_value = 0;

	pe[rr][ee].add_enable_curr = 0;
	pe[rr][ee].add_enable_past = 0;

	pe[rr][ee].add_value = 0;
	pe[rr][ee].add_cnt = 0;

}