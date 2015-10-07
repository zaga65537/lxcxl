//
//  prob_utils.c
//  lxcxl
//
//  Created by zhang yunshui on 10/4/15.
//  Copyright © 2015 ZhangYunshui. All rights reserved.
//

#include "prob_utils.h"


double* pq_alloc(double* m_rates, double* lapse, int b_term, int ad_num, int ci_num, int cid_num){
    //根据死亡率计算PQ
    //m_rates		中各死亡率的储存顺序为总死亡率(1), 意外发生率(ad_num), 重疾发生率(ci_num), 重疾死亡率站总死亡率百分比(ci_num),注意m_rates中数据是按列存。
    //lapse 脱落率
    int row_num = 1 + ad_num + ci_num * 2;
    int input_num = b_term * row_num;
    int output_num = input_num + b_term * 2; // 多一列生存的概率, 以及一列脱落率

    
    double *res = (double*) calloc(output_num, sizeof(double)); // 在堆上分配返回数组的内存并初始化为0
    
    // 各类型保险责任概率指针
    double *p_d_rate = m_rates;
    double *p_ad_rate = m_rates + b_term;
    double *p_ci_rate = p_ad_rate + b_term * ad_num;
    double *p_cid_rate = p_ci_rate + b_term * ci_num;
    double *p_p = res;	// 生存率列
    double *p_q = p_p + b_term;	// 总死亡率列
    double *p_q_ad = p_q + b_term;	// 意外伤害责任列
    double *p_q_ci = p_q_ad + b_term * ad_num; // 重疾率列
    double *p_q_cid = p_q_ci + b_term * ci_num; // 重疾死亡率列
    double *p_q_lapse; // 脱落率
    if (lapse == NULL) {
        p_q_lapse = NULL;
    }else{
        p_q_lapse = p_q_cid + b_term * cid_num;
    }
    
    
    // 循环所需数据的声明和初始化
    double active_holder = 1;
    double living = 1;
    double new_dead, new_ad, new_ci, new_cid, new_lps;
    
    for(int i=0; i<b_term; ++i){
        p_p[i] = active_holder;	// 记录保单留人数
        new_dead = active_holder * p_d_rate[i];	// 在保内新死亡人数
        
        // 计算新发生意外人数
        new_ad = 0;
        for(int j=0; j<ad_num; ++j){
            new_ad += p_ad_rate[i + j*b_term] * active_holder;
            // 记录意外人数
            p_q_ad[i + b_term*j] = p_ad_rate[i + j*b_term] * active_holder;
        }
        
        // 计算新生病人数
        new_ci = 0;
        new_cid = 0;
        for(int j=0; j<ci_num; ++j){
            p_q_ci[i + j*b_term] = active_holder * p_ci_rate[i + j*b_term];
            new_ci += active_holder * p_ci_rate[i + j*b_term];
        }
        
        for (int j=0; j<cid_num; ++j) {
            p_q_cid[i + j*b_term] = living * p_cid_rate[i+j*b_term];
            new_cid += living * p_cid_rate[i+j*b_term];

        }
        
        p_q[i] = new_dead;
        active_holder -= (new_dead + new_ci);
        living -= new_dead;
        if(lapse != NULL){

            new_lps = lapse[i] * active_holder;
            active_holder -= new_lps;
            living -= new_lps;
            p_q_lapse[i] = new_lps;
         // printf("lapse is %i, %f, new_lps, %f, %f \n", i, lapse[i], new_lps, p_q_lapse[i]);
        }
    }
    
    return res;
    
}
