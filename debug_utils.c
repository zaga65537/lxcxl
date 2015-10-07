//
//  debug_utils.c
//  lxcxl
//
//  Created by zhang yunshui on 10/4/15.
//  Copyright © 2015 ZhangYunshui. All rights reserved.
//

#include "debug_utils.h"


double * new_array_by(int val, int length){
    double *rst = (double *) malloc(sizeof(double)*length);
    for(int i=0; i<length; ++i){
        rst[i] = val;
    }
    return rst;
}

double * copy_array(double* from, int length){
    double *rst = (double *) malloc(sizeof(double)*length);
    for(int i=0; i<length; ++i){
        rst[i] = from[i];
    }
    return rst;
}
