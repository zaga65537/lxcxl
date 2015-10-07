//
//  debug_utils.h
//  lxcxl
//
//  Created by zhang yunshui on 10/4/15.
//  Copyright © 2015 ZhangYunshui. All rights reserved.
//

#ifndef debug_utils_h
#define debug_utils_h

#include <stdio.h>
#include "prob_utils.h"
#include <assert.h>

//================== for leaning ctypes ==================
double * new_array_by(int val, int length);
double * copy_array(double* from, int length);
#endif /* debug_utils_h */
