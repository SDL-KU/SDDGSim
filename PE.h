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

#include "FIFO.h"

class PE {

public:
	///////////////////////Eyeriss: PE variables///////////////////////\

	//PE variables
	int ifmap_data_from_fifo, fmap_data_from_fifo, ofmap_data_from_fifo;

	bool ifmap_valid, fmap_valid, ofmap_valid_gin;
	bool ofmap_ready_gon;


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


	///////////////////////Eyeriss: PE variables///////////////////////
	void pe_member_var_initializer();
	void main_ctrl(bool& ofmap_gon_valid);
	void main_ctrl_action();
	void mult();
	void mult_mux();
	void psum_mux();
	void add(int& ofmap);
	void fifo_to_spad(int r_index, int& ofmap);
	void setting(int fifo_size0, int fifo_size1, int fifo_size2);

private:
	///////////////////////Eyeriss: PE Function///////////////////////

	// subclass 
	FIFO<int> fifo_ifmap, fifo_fmap, fifo_ofmap_gin;

	// update variables			
	/*bool ifmap_valid, fmap_valid, ofmap_valid_gin;
	bool ofmap_ready_gon;


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
	int add_cnt_ff;*/

public:
	void reset();

	void action(int wdata0, int wdata1, int wdata2, bool wvalid0, bool wvalid1, bool wvalid2, bool rready, int r_index)
	{		
		main_ctrl_action();
		
		//GLB --> FIFO
		fifo_ifmap.action(wdata0, wvalid0, ifmap_ready_curr);
		fifo_fmap.action(wdata1, wvalid1, fmap_ready);
		fifo_ofmap_gin.action(wdata2, wvalid2, ofmap_gin_ready_curr);

		ofmap_ready_gon = rready;
	}

	void update(bool& wready_ifmap, bool& wready_fmap, bool& wready_ofmap, int &ofmap, bool& ofmap_gon_valid, int r_index, int e_index, int curr_cycle)
	{
		//int tmp;

		fifo_ifmap.update(wready_ifmap, ifmap_data_from_fifo, ifmap_valid);                    // => FIFO to PE
		fifo_fmap.update(wready_fmap, fmap_data_from_fifo, fmap_valid);                        // => FIFO to PE
		fifo_ofmap_gin.update(wready_ofmap, ofmap_data_from_fifo, ofmap_valid_gin);		   // => FIFO to PE

		if (curr_cycle != -1) {
			main_ctrl(ofmap_gon_valid);



			//fifo_to_spad(r_index);
		}
	}
};

void PE::pe_member_var_initializer()
{
	//PE variables
	ifmap_data_from_fifo = NON_INITIAL;
	fmap_data_from_fifo  = NON_INITIAL;
	ofmap_data_from_fifo = NON_INITIAL;

	for (int i = 0; i < q*S; i++) {
		spad_ifmap[i] = NON_INITIAL;
	}
	for (int i = 0; i < q*p*S; i++) {
		spad_fmap[i]  = NON_INITIAL;
	}
	for (int i = 0; i < p; i++) {
		spad_ofmap[i] = NON_INITIAL;
	}

	////////////      test
	ofmap_gon_valid_curr=0;
	ofmap_gon_valid_past=0;

	ifmap_ready_next = 1;
	ifmap_ready_curr = 1;

	ifmap_q_cnt = 0;
	ifmap_offset_cnt = 0;


	fmap_ready = 1;

	ofmap_gin_ready_next = 1;
	ofmap_gin_ready_curr = 1;


	i_r_ptr = 0;
	i_t_ptr = 0;
	i_t_ptr_offset = q;
	i_r_ptr_cnt = 0;

	f_r_ptr = 0;
	f_t_ptr = 0;
	f_r_ptr_cnt = 0;

	o_r_ptr = 0;
	o_t_ptr = 0;
	o_r_ptr_cnt = 0;


	spad_ifmap_full_flag = 0;
	spad_fmap_full_flag = 0;


	ifmap_handshake_curr = 0;
	ifmap_handshake_past = 0;

	ifmap_handshake_cnt = 0;

	fmap_handshake_curr = 0;
	fmap_handshake_past = 0;

	ofmap_gin_handshake_curr = 0;
	ofmap_gin_handshake_past = 0;

	ofmap_gon_handshake_curr = 0;
	ofmap_gon_handshake_past = 0;

	ofmap_gin_handshake_cnt = 0;

	ofmap_gin_handshake_cnt_temp=0;

	ofmap_gon_handshake_cnt = 0;

	mult_enable_curr = 0;
	mult_enable_past = 0;

	mult_cnt = 0;
	mult_value = 0;

	mult_cnt_temp = 0;

	mult_mux_sel = 0;
	psum_mux_sel = 0;

	psum_mux_sel_cnt = 0;

	mult_mux_value = 0;
	psum_mux_value = 0;

	add_enable_curr = 0;
	add_enable_past = 0;

	add_value = 0;
	add_cnt = 0;

}

void PE::main_ctrl_action() {

	ofmap_gon_valid_past = ofmap_gon_valid_curr;

	ifmap_ready_curr = ifmap_ready_next;
	ofmap_gin_ready_curr = ofmap_gin_ready_next;

	ifmap_handshake_past = ifmap_handshake_curr;
	fmap_handshake_past = fmap_handshake_curr;
	ofmap_gin_handshake_past = ofmap_gin_handshake_curr;
	ofmap_gon_handshake_past = ofmap_gon_handshake_curr;

	mult_enable_past = mult_enable_curr;
	mult_cnt_ff = mult_cnt;
	add_enable_past = add_enable_curr;
	add_cnt_ff = add_cnt;
}


void PE::main_ctrl(bool& ofmap_gon_valid)
{
	///////////////////////////////             ifmap spad ctrl.

	//receive indicator
	if (ifmap_handshake_past)
	{
		i_r_ptr = (++i_r_ptr_cnt) % (q*S);
	}

	if (ifmap_ready_curr && ifmap_valid)
	{
		ifmap_handshake_curr = 1;

		ifmap_handshake_cnt++;

		if (ifmap_handshake_cnt == q * S)
			spad_ifmap_full_flag = 1;

		if (spad_ifmap_full_flag) {
			ifmap_q_cnt = ifmap_handshake_cnt % q;
			if (ifmap_q_cnt == 0)
				ifmap_offset_cnt++;
		}
	}
	else
		ifmap_handshake_curr = 0;


	//trans indicator

	if (mult_enable_past) {
		if (mult_cnt_ff%p == 0) {
			if (mult_cnt_ff % (p*q*S) == 0 && mult_cnt_ff != 0) {
				i_t_ptr = ((++i_t_ptr) + i_t_ptr_offset) % (q*S);
			}
			else
				i_t_ptr = (++i_t_ptr) % (q*S);
		}
	}

	///////////////////////////////             fmap spad ctrl

	//receive indicator
	if (fmap_handshake_past)
	{
		f_r_ptr = (++f_r_ptr_cnt) % (p*q*S);
	}



	if (fmap_ready && fmap_valid)
	{
		fmap_handshake_curr = 1;

		if (f_r_ptr == p * q*S - 1)
			spad_fmap_full_flag = 1;
	}
	else
		fmap_handshake_curr = 0;



	//trans indicator
	if (mult_enable_past) {
		f_t_ptr = (++f_t_ptr) % (p*q*S);
	}


	///////////////////////////////             ofmap spad ctrl
	//receive indicator
	if (ofmap_gin_handshake_past)
	{
		o_r_ptr = (++o_r_ptr_cnt) % p;
	}


	if (ofmap_gin_ready_curr && ofmap_valid_gin) {
		ofmap_gin_handshake_curr = 1;

		ofmap_gin_handshake_cnt++;
	}
	else
		ofmap_gin_handshake_curr = 0;


	//trans indicator
	if (mult_enable_past) {
		o_t_ptr = (++o_t_ptr) % p;
	}


	///////////////////////////////             Mult enable signal
	if (mult_cnt_ff < p*q*S*F)
	{
		if (ofmap_gon_valid_past && !ofmap_gon_handshake_past)
			mult_enable_curr = 0;
		else {
			if (!ofmap_gin_handshake_curr) {
				//if (   (((mult_cnt/(p*q*S) )-1)  *p)  +(mult_cnt+1) > add_cnt             ) {
				if (mult_cnt_ff % (p*q*S) == 0 && add_cnt_ff % (p*q*S + p) >= p * q*S && add_cnt_ff % (p*q*S + p) <= p * q*S + p - 1) {

					mult_enable_curr = 0;

				}
				else {
					//if (!spad_ifmap_full_flag || !spad_fmap_full_flag) {
					if (i_r_ptr_cnt < q*S || f_r_ptr_cnt < p*q*S) {
						if (mult_cnt_ff < p*i_r_ptr_cnt && mult_cnt_ff < f_r_ptr_cnt) {
							mult_enable_curr = 1;
						}
						else {
							mult_enable_curr = 0;
						}
					}
					else {
						if (i_r_ptr != i_t_ptr) {
							mult_enable_curr = 1;
						}
						else {
							if (ifmap_ready_curr == 0) {
								mult_enable_curr = 1;
							}
							else
								mult_enable_curr = 0;
						}
					}
				}
			}
			else
				mult_enable_curr = 0;
		}
	}
	else
		mult_enable_curr = 0;

	if (mult_enable_curr) {
		mult_cnt++;
	}

	///////////////////////////////             mult mux signal

	if (mult_enable_curr)
		mult_mux_sel = 0;
	else if (ofmap_gin_handshake_curr)
		mult_mux_sel = 1;
	else
		mult_mux_sel = 0;

	///////////////////////////////             add enable signal
	//add
	if (mult_enable_curr || ofmap_gin_handshake_curr) {
		add_enable_curr = 1;
		add_cnt++;
	}
	else
		add_enable_curr = 0;






	///////////////////////////////             psum mux signal
	if (psum_mux_sel_cnt < p) {
		if (ofmap_gin_handshake_curr) {
			if (ofmap_gin_handshake_cnt%p == 0) {
				psum_mux_sel = 1;
			}
			else if (mult_cnt % (p*q*S) < ofmap_gin_handshake_cnt % p)
				psum_mux_sel = 1;
			else
				psum_mux_sel = 0;
		}
		else if (mult_enable_curr) {
			if (ofmap_gin_handshake_cnt%p == 0) {
				psum_mux_sel = 1;
			}
			else if (mult_cnt % (p*q*S) > ofmap_gin_handshake_cnt % p)
				psum_mux_sel = 1;
			else
				psum_mux_sel = 0;
		}
		else
			psum_mux_sel = 0;
	}
	else
		psum_mux_sel = 0;

		

	
	if (add_cnt%(p*q*S + p) == 0)
		psum_mux_sel_cnt = 0;
	else if (psum_mux_sel)
		psum_mux_sel_cnt++;
	





	/////////////////////////////////    ofmap_gon_valid signal
	if (mult_cnt != 0 && mult_cnt % (p*q*S) == 0) {
		if ((mult_cnt - 1) / (p*q*S) == (ofmap_gin_handshake_cnt - 1) / p)
			mult_cnt_temp = p * q*S;
		else
			mult_cnt_temp = mult_cnt % (p*q*S);
	}
	else
		mult_cnt_temp = mult_cnt % (p*q*S);



	if (ofmap_gin_handshake_cnt != 0 && ofmap_gin_handshake_cnt % (p) == 0) {
		if ((mult_cnt - 1) / (p*q*S) == (ofmap_gin_handshake_cnt - 1) / p)
			ofmap_gin_handshake_cnt_temp = p;
		else
			ofmap_gin_handshake_cnt_temp = ofmap_gin_handshake_cnt % (p);
	}
	else
		ofmap_gin_handshake_cnt_temp = ofmap_gin_handshake_cnt % (p);




	if (mult_enable_curr) {
		if (mult_cnt_temp >= p * q*S - p + o_t_ptr + 1 && ofmap_gin_handshake_cnt_temp >= o_t_ptr + 1)
			ofmap_gon_valid_curr = 1;
		else
			ofmap_gon_valid_curr = 0;
	}
	else if (ofmap_gin_handshake_curr) {
		if (mult_cnt_temp >= p * q*S - p + o_r_ptr + 1 && ofmap_gin_handshake_cnt_temp >= o_r_ptr + 1)
			ofmap_gon_valid_curr = 1;
		else
			ofmap_gon_valid_curr = 0;
	}
	else
		ofmap_gon_valid_curr = 0;

	ofmap_gon_valid = ofmap_gon_valid_curr;





	///////////////////////////////             ofmap handshake

	//ofmap_gon_handshake
	if (ofmap_gon_valid_curr && ofmap_ready_gon) {
		ofmap_gon_handshake_curr = 1;
		ofmap_gon_handshake_cnt++;
	}
	else
		ofmap_gon_handshake_curr = 0;
	

	///////////////////////////////             spad_to_fifo_ready signal

	//    ifmap_ready
	if (spad_ifmap_full_flag)
	{
		if (mult_cnt >= (ifmap_q_cnt*p + (ifmap_offset_cnt - 1)*p*q*S + p))
			ifmap_ready_next = 1;
		else
			ifmap_ready_next = 0;
	}
	else {
		ifmap_ready_next = 1;
	}



	//    fmap_ready
	fmap_ready = 1;

	//    ofmap_gin_ready
	//if(add_cnt%(p*q*S+p)==0 && ofmap_gon_handshake_cnt%p==0&& ofmap_gon_handshake_curr)
	if(add_cnt%(p*q*S+p)==0)
		ofmap_gin_ready_next = 1;

	else if(o_r_ptr == p - 1 && ofmap_gin_handshake_curr)		
		ofmap_gin_ready_next = 0;

}


void PE::mult() {
	//mult
	mult_value = spad_ifmap[i_t_ptr] * spad_fmap[f_t_ptr];
}

void PE::mult_mux() {
	//mult_mux
	if (!mult_mux_sel)
		mult_mux_value = mult_value;
	else
		mult_mux_value = ofmap_data_from_fifo;
}

void PE::psum_mux() {
	//psum_mux
	if (!psum_mux_sel) {
		if (ofmap_gin_handshake_curr)
			psum_mux_value = spad_ofmap[o_r_ptr];
		else
			psum_mux_value = spad_ofmap[o_t_ptr];
	}
	else
		psum_mux_value = 0;
}

void PE::add(int& ofmap) {
	int temp;

	if (ofmap_gin_handshake_curr)
		temp = o_r_ptr;
	else
		temp = o_t_ptr;

	//add
	add_value = mult_mux_value + psum_mux_value;

	//if (!pe[0][ee].ofmap_gon_valid_curr) {
	//	if (pe[0][ee].ofmap_gin_handshake_curr)
	//		pe[0][ee].spad_ofmap[temp] = output_id;
	//	else
	//		pe[0][ee].spad_ofmap[temp] = pe[0][ee].spad_ofmap[temp];
	//}
	//else
	//	ofmap = add_value;

	//if (!ofmap_gon_valid_curr) {
	//	if (add_enable_curr)
	//			spad_ofmap[temp] = output_id;
	//	else
	//		spad_ofmap[temp] = spad_ofmap[temp];
	//}
	//else
	//	ofmap = add_value;
}

void PE::fifo_to_spad(int r_index, int& ofmap)
{
	int temp;

	if (ifmap_handshake_curr)
		spad_ifmap[i_r_ptr] = ifmap_data_from_fifo;
	if (fmap_handshake_curr)
		spad_fmap[f_r_ptr] = fmap_data_from_fifo;

	if (ofmap_gin_handshake_curr)
		temp = o_r_ptr;
	else
		temp = o_t_ptr;

	if (!ofmap_gon_valid_curr) {
		if (ofmap_gin_handshake_curr) {
			spad_ofmap[temp] = ofmap_data_from_fifo;
			//cout << r_index << endl;
			//cout << "ofmap_fifo :" << ofmap_data_from_fifo << endl;
		}
		else
		{
			spad_ofmap[temp] = spad_ofmap[temp];
		}
	}
	else {
		if (ofmap_gin_handshake_curr)
		{
			spad_ofmap[temp] = ofmap_data_from_fifo;
			ofmap = spad_ofmap[temp];
		}
		else
		{
			ofmap = spad_ofmap[temp];
		}
	}

}


// Default
void PE::reset() {
	fifo_ifmap.reset();
	fifo_fmap.reset();
	fifo_ofmap_gin.reset();
}

void PE::setting(int fifo_size0, int fifo_size1, int fifo_size2)
{
	fifo_ifmap.setting(fifo_size0);
	fifo_fmap.setting(fifo_size1);
	fifo_ofmap_gin.setting(fifo_size2);
}