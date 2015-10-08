#include "gp_utils.h"

double * load_benefit(BenefitData* b_arr, int b_arr_len, Contract *contract) {
	int res_len = contract->b_term;
	double *res = (double*) calloc(res_len, sizeof(double));
	int age = contract->issue_age;
	BenefitData *p_benefit;
	int start_policy_year, end_policy_year;
	for(int i=0; i<b_arr_len; ++i){
		p_benefit = b_arr + i;
		if(p_benefit->b_age){
		  start_policy_year = p_benefit->begin - age + 1;
		}else{
		  start_policy_year = p_benefit->begin;
		}
		if(p_benefit->e_age){
		  end_policy_year = p_benefit->end - age + 1;
		}else{
		  end_policy_year = p_benefit->end;
		}

		if (end_policy_year > res_len){
			end_policy_year = res_len;
		}

		for(int j=start_policy_year; j<=end_policy_year; ++j){
		  res[j-1] += p_benefit->data;
		}
	}
	return res;
}


double * actuarial_pv(double *pay, int pay_len, double *prob, double rate, double when) {
	double* res = (double*) malloc(pay_len * sizeof(double));
	memcpy(res, pay, pay_len*sizeof(double));
	double discount = 1 / (1 + rate);
	if (prob == NULL) {
		for(int i=0; i<pay_len; ++i) {
			res[i] *= pow(discount, i + when);
		}
	}else {
		for(int i=0; i<pay_len; ++i) {
			res[i] *= pow(discount, i + when) * prob[i];
		}
	}
	return res;
}


double * loading(double *loading_arr, int row_num, int col_num, int *p_term_key, int key_len, int p_term) {
	// loading_arr 按行存， 每行表示一个p_term选项下各年的loading
	// warn
	int r_pos = int_find_arr(p_term_key, key_len, p_term);
	if (r_pos < 0){
		if (p_term > p_term_key[key_len-1]){
			r_pos = key_len - 1;
		}else{
			return NULL;
		}

	}
	double * res = (double*) malloc(p_term*sizeof(double));
	double * p_loading = loading_arr + r_pos * col_num;
	if (p_term > col_num){
		memcpy(res, p_loading, col_num * sizeof(double));
		for(int i=col_num; i<p_term; ++i){
			res[i] = p_loading[col_num-1];
		}
	}else{
		memcpy(res, p_loading, p_term * sizeof(double));
	}

	return res;
}
