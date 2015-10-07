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


double * gp(double *pq_arr, int b_term, int ad_num, int ci_num, int cid_num, double** benefit, double** benefit_gp, double* when, double *loading, double rate) {
	// benefit: DB,ADB1,ADB2,ADB3,ADB4,CIB,SB,MB
	// 注意没有cidb
	double *p_p = pq_arr;
	double *p_q = p_p + b_term;
	double *p_ad = p_q + b_term;
	double *p_ci = p_ad + b_term * ad_num;
	double *p_cid = p_ci + b_term * ci_num;
	
	// db
	double *p_db = actuarial_pv(benefit[0], b_term, p_q, rate, 0.5); // need to free
	double *p_db_gp = actuarial_pv(benefit_gp[0], b_term, p_q, rate, 0.5); // need to free
	// adb
	double **pp_adb = (double **) malloc(ad_num * sizeof(double*)); // need to free
	double **pp_adb_gp = (double **) malloc(ad_num * sizeof(double*));
	for(int i=0; i<ad_num; ++i) {
		pp_adb[i] = actuarial_pv(benefit[1+i], b_term, p_ad + i*b_term, rate, 0.5); // need to free
		pp_adb_gp[i] = actuarial_pv(benefit_gp[1+i], b_term, p_ad + i*b_term, rate, 0.5); // need to free
	}
	
	// cib
	double **pp_cib = (double **) malloc(ci_num * sizeof(double*)); // need to free
	double **pp_cib_gp = (double **) malloc(ci_num * sizeof(double*)); // need to free
	for(int i=0; i<ad_num; ++i) {
		pp_cib[i] = actuarial_pv(benefit[1+ad_num+i], b_term, p_ci + i*b_term, rate, 0.5); // need to free
		pp_cib_gp[i] = actuarial_pv(benefit_gp[1+ad_num+i], b_term, p_ci + i*b_term, rate, 0.5); // need to free
	}
	
	// sb
	double *p_sb = actuarial_pv(benefit[1+ad_num+ci_num], b_term, p_p, rate, 0); // need to free
	double *p_sb_gp = actuarial_pv(benefit_gp[1+ad_num+ci_num], b_term, p_p, rate, 0); // need to free
	// mb 
	double *p_mb = actuarial_pv(benefit[2+ad_num+ci_num], b_term, p_p, rate, 1); // need to free
	double *p_mb_gp = 
	
	// free
	free(p_db); free(p_db_gp); 
	for(int i=0; i<ad_num; ++i){
		free(pp_adb[i]);
		free(pp_adb_gp[i]);
	}
	free(pp_adb);
	free(pp_adb_gp);
	for(int i=0; i<ci_num; ++i){
		free(pp_cib[i]);
		free(pp_cib_gp[i]);
	}
	free(pp_cib);
	free(pp_cib_gp);
	
}