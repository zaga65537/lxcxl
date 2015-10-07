import ctypes as c
import numpy as np

libc = c.CDLL("../lxcxl.so")
Male, Female = 0, 1
DB, ADB1, ADB2, ADB3, ADB4, CIB, SB, MB, DB_GP, ADB_GP1, ADB_GP2, ADB_GP3, ADB_GP4, CIB_GP, SB_GP, MB_GP = \
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

BenefitNameDict = {"DB": DB, "ADB1": ADB1, "ADB2": ADB2, "ADB3": ADB3, "ADB4": ADB4, "CIB": CIB, "SB": SB, "MB": MB, "DB_GP": DB_GP, "ADB_GP1" :ADB_GP1, "ADB_GP2": ADB_GP2,
                   "ADB_GP3": ADB_GP3, "ADB_GP4": ADB_GP4, "CIB_GP": CIB_GP, "SB_GP": SB_GP, "MB_GP": MB_GP}


class Contract(c.Structure):
    """
    typedef struct {
        Gender gender;
        int issue_age;
        int policy_year;
        int b_term; // benefit term
        int p_term; // payment term
    }Contract;
    """
    _fields_ = [("gender", c.c_int),
                ("issue_age", c.c_int),
                ("policy_year", c.c_int),
                ("b_term", c.c_int),
                ("p_term", c.c_int)]


class BenefitData(c.Structure):
    """
    typedef  struct {
        enum BenefitType type;
        double data;
        int from;
        int to;
        bool f_age;
        bool t_age;
    }BenefitData;
    """
    _fields_ = [("type", c.c_int),
                ("data", c.c_double),
                ("begin", c.c_int),
                ("end", c.c_int),
                ("b_age", c.c_bool),
                ("e_age", c.c_bool)]


def pq(m_rates, lapse, b_term, n_ad, n_ci, n_cid):
    c_fun = libc.pq_alloc
    c_fun.argtypes = [c.POINTER(c.c_double), c.POINTER(c.c_double), c.c_int, c.c_int, c.c_int]
    c_fun.restype = c.POINTER(c.c_double)
    if lapse is not None:
        p_rst = c_fun(np.ctypeslib.as_ctypes(np.ravel(m_rates, "F")), np.ctypeslib.as_ctypes(np.ravel(lapse, "F")),
                      b_term, n_ad, n_ci, n_cid)
    else:
        p_rst = c_fun(np.ctypeslib.as_ctypes(np.ravel(m_rates, "F")), None, b_term, n_ad, n_ci, n_cid)
    return np.ctypeslib.as_array(p_rst, (3 + n_ad + n_ci + n_cid, b_term)).T


def load_benefit(benefits_ndarr, contract):
    c_fun = libc.load_benefit
    c_fun.argtypes = [c.POINTER(BenefitData), c.c_int, c.POINTER(Contract)]
    c_fun.restype = c.POINTER(c.c_double)
    b_arr_len = benefits_ndarr.shape[0]
    b_arr_type = BenefitData * b_arr_len
    b_arr = b_arr_type()
    for i in range(b_arr_len):

        t_type = benefits_ndarr[i, 0]

        if isinstance(t_type, str):
            t_type = BenefitNameDict[t_type]

        b_arr[i] = BenefitData(type=t_type, data=benefits_ndarr[i, 1],
                               begin=benefits_ndarr[i, 2], end=benefits_ndarr[i, 3],
                               b_age=(benefits_ndarr[i, 4] == 1), e_age=(benefits_ndarr[i, 5] == 1))
    res = c_fun(b_arr, b_arr_len, c.pointer(contract))
    return np.ctypeslib.as_array(res, (contract.b_term,))


def actuarial_pv(pay_ndarr, rate, when, prob_ndarr=None):
    """
    double * actuarial_pv(double *pay, int pay_len, double *prob, double rate, double when);
    :param pay_ndarr:
    :param rate:
    :param when:
    :param prob_ndarr:
    :return:
    """
    c_fun = libc.actuarial_pc
    c_fun.argtypes = [c.POINTER(c.c_double), c.c_int, c.POINTER(c.c_double), c.c_double, c.c_double]
    c_fun.restype = c.POINTER(c.c_double)
    if prob_ndarr is None:
        res = c_fun(np.ctypeslib.as_ctypes(pay_ndarr), pay_ndarr.size, None, rate, when)
    else:
        res = c_fun(np.ctypeslib.as_ctypes(pay_ndarr), pay_ndarr.size, np.ctypeslib.as_ctypes(prob_ndarr), rate, when)
    return np.ctypeslib.as_array(res, (pay_ndarr.size, ))
