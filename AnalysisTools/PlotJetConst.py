import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets.csv',header=None)
df_bkg = pd.read_csv('../ConversionTools/JetInfo_qcd.csv',header=None)


dfSigConst = df_sig.iloc[:,1:]
dfbkgConst = df_bkg.iloc[:,1:]

sigConsts = df_sig.values.flatten()
bkgConsts = df_bkg.values.flatten()

fig = plt.figure()
ax = fig.add_subplot(2,1,1)
ax.hist(sigConsts,1000,[0.0001,1])
ax.set_yscale('log')
fig.savefig('SigConstDist.pdf')
ax.hist(bkgConsts,1000,[0.0001,1])
ax.set_yscale('log')
#plt.hist(bkgConsts,1000,[0.0001,1])
fig.savefig('BkgConstDist.pdf')
