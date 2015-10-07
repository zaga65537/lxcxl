import pandas as pd
from py_api import *


def load_m_rates(gender, age=0, b_term = 20):
    df = pd.read_csv("staticdata.csv")
    if gender == Male:
        return df[["D_M", "AD1", "AD2", "AD3", "AD4", "CI_M", "CID_M"]][age:age+b_term+1]
    else:
        return df[["D_F", "AD1", "AD2", "AD3", "AD4", "CI_F", "CID_F"]][age:age+b_term+1]


def load_benefit_df(df, name):
    return df[df["Type"] == name]


def test_pq():
    df_rates = load_m_rates(Male, 60, 20)
    nda_rates = df_rates.values
    b_term = nda_rates.shape[0]
    n_ad = 4
    n_ci, n_cid = 1, 1
    lapse = np.zeros((b_term,))
    pq_res = pq(nda_rates, lapse, b_term, n_ad, n_ci, n_cid)
    print pq_res
    pd.DataFrame(pq_res, columns=("P", "Q", "AD1", "AD2", "AD3", "AD4", "CI", "CID", "lapse")).to_csv("out_pq.csv",
                                                                                                      index=False)


def test_load_benefit(contract):
    df = pd.read_csv("benefit.csv")
    benefit_names = ("DB", "ADB1", "ADB2", "ADB3", "ADB4", "CIB", "SB", "MB", "DB_GP", "ADB_GP1", "ADB_GP2",
                     "ADB_GP3", "ADB_GP4", "CIB_GP", "SB_GP", "MB_GP")
    b_dfs = [load_benefit_df(df, name) for name in benefit_names]
    b_ndarr = [pd.DataFrame(data=load_benefit(df.values, contract), columns=(name,)) for df, name in
               zip(b_dfs, benefit_names)]
    res = reduce(lambda x, y: pd.merge(x, y, left_index=True, right_index=True), b_ndarr)
    return res


if __name__ == "__main__":
    test_pq()
    # contract = Contract(Male, 60, 1, 20, 5)
    # df = test_load_benefit(contract)
    # df.to_csv("out_benefit.csv", index=False)
