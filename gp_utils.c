#include "gp_utils.h"

double * load_benefit(BenefitData* b_arr, int b_arr_len, Contract *contract) {
	int res_len = contract->b_term;
	double *res = (double*) calloc(res_len, sizeof(double));
	int age = contract->issue_age;
	BenefitData *p_benefit;
	int start_policy_year, end_policy_year;
	for(size_t i = 0; i < b_arr_len; ++i){
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

		for(size_t j = start_policy_year; j <= end_policy_year; ++j){
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
		for(size_t i = 0; i < pay_len; ++i) {
			res[i] *= pow(discount, i + when);
		}
	}else {
		for(size_t i = 0; i < pay_len; ++i) {
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
		for(size_t i = col_num; i < p_term; ++i){
			res[i] = p_loading[col_num-1];
		}
	}else{
		memcpy(res, p_loading, p_term * sizeof(double));
	}

	return res;
}


double  gross_premium(double *pq_arr, int b_term, int p_term, int ad_num,
		int ci_num, int cid_num, double* benefit, double* benefit_gp, double* when, double *loading, double rate){

  //"DB", "ADB1", "ADB2", "ADB3", "ADB4", "CIB", "SB", "MB", "DB_GP", "ADB_GP1", "ADB_GP2", "ADB_GP3", "ADB_GP4", "CIB_GP", "SB_GP", "MB_GP"
	int pq_row_num = b_term + 1; // pq 要比benefit term 多计算一年；
	double numerator = 0;
	double denominator = 0;

	// DB
	double *focus_benefit = benefit;
	double *focus_benefit_gp = benefit_gp;
	double *prob = pq_arr + pq_row_num;
	focus_benefit = actuarial_pv(focus_benefit, b_term, prob, rate, 0.5);
	focus_benefit_gp = actuarial_pv(focus_benefit_gp, b_term, prob, rate, 0.5);
	numerator += ys_sum(focus_benefit, b_term);
	denominator -= ys_sum(focus_benefit_gp, b_term);
	free(focus_benefit); free(focus_benefit_gp);

	// ADB

	for (size_t i = 0; i < ad_num; ++i) {
		focus_benefit = benefit + b_term * (i + 1);
		focus_benefit_gp = benefit_gp + b_term * (i + 1);
		prob = pq_arr + pq_row_num * (i + 2);
		focus_benefit = actuarial_pv(focus_benefit, b_term, prob, rate, 0.5);
		focus_benefit_gp = actuarial_pv(focus_benefit_gp, b_term, prob, rate, 0.5);
		numerator += ys_sum(focus_benefit, b_term);
		denominator -= ys_sum(focus_benefit_gp, b_term);
		free(focus_benefit); free(focus_benefit_gp);
	}

	// CIB

	for (size_t i = 0; i < ci_num; ++i) {
		focus_benefit = benefit + b_term * (i + ad_num + 1);
		focus_benefit_gp = benefit_gp + b_term * (i + ad_num + 1);
		prob = pq_arr + pq_row_num * (i + 2 + ad_num);
		focus_benefit = actuarial_pv(focus_benefit, b_term, prob, rate, 0.5);
		focus_benefit_gp = actuarial_pv(focus_benefit_gp, b_term, prob, rate, 0.5);
		numerator += ys_sum(focus_benefit, b_term);
		denominator -= ys_sum(focus_benefit_gp, b_term);
		free(focus_benefit); free(focus_benefit_gp);
	}

	// SB

	focus_benefit = benefit + b_term * (ad_num + ci_num + 1);
	focus_benefit_gp = benefit_gp + b_term * (ad_num + ci_num + 1);
	prob = pq_arr;
	focus_benefit = actuarial_pv(focus_benefit, b_term, prob, rate, 0);
	focus_benefit_gp = actuarial_pv(focus_benefit_gp, b_term, prob, rate, 0);
	numerator += ys_sum(focus_benefit, b_term);
	denominator -= ys_sum(focus_benefit_gp, b_term);
	free(focus_benefit); free(focus_benefit_gp);

	// MB

	focus_benefit = benefit + b_term * (ad_num + ci_num + 2);
	focus_benefit_gp = benefit_gp + b_term * (ad_num + ci_num + 2);
	prob = pq_arr + 1;
	focus_benefit = actuarial_pv(focus_benefit, b_term, prob, rate, 1);
	focus_benefit_gp = actuarial_pv(focus_benefit_gp, b_term, prob, rate, 1);
	numerator += ys_sum(focus_benefit, b_term);
	denominator -= ys_sum(focus_benefit_gp, b_term);
	free(focus_benefit); free(focus_benefit_gp);

	// loading
	focus_benefit = (double*) malloc(p_term*sizeof(double));
	memcpy(focus_benefit, loading, p_term*sizeof(double));
	for (size_t i = 0; i < p_term; ++i) {
		focus_benefit[i] = 1 - focus_benefit[i];
	}
	focus_benefit_gp = actuarial_pv(focus_benefit, p_term, prob - 1, rate, 0);
	denominator += ys_sum(focus_benefit_gp, p_term);
	free(focus_benefit); free(focus_benefit_gp);


  return numerator / denominator;
}
