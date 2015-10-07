//
//  gp_utils.h
//  lxcxl
//
//  Created by zhang yunshui on 10/5/15.
//  Copyright © 2015 ZhangYunshui. All rights reserved.
//

#ifndef gp_utils_h
#define gp_utils_h

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {
    DB, ADB1, ADB2, ADB3, ADB4, CIB, SB, MB,
    DB_GP, ADB_GP1, ADB_GP2, ADB_GP3, ADB_GP4, CIB_GP, SB_GP, MB_GP

}BenefitType;

typedef enum {
    Male=0,
    Female
}Gender;

typedef struct {
    BenefitType type;
    double data;
    int begin;
    int end;
    bool b_age;
    bool e_age;
}BenefitData;


typedef struct {
    Gender gender;
    int issue_age;
    int policy_year;
    int b_term; // benefit term
    int p_term; // payment term
}Contract;

double * load_benefit(BenefitData* b_arr, int b_arr_len, Contract *contract);
double * actuarial_pv(double *pay, int pay_len, double *prob, double rate, double when);
double * gp(double *pq_arr, int b_term, int ad_num, int ci_num, int cid_num, double** benefit, double** benefit_gp, double* when, double* loading);

#endif /* gp_utils_h */