#!/usr/env/python
import pandas as pd
import numpy as np
from CNN import *

#open csv files
df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets.csv',header=None)
df_bkg = pd.read_csv('../ConversionTools/JetInfo_qcd.csv',header=None)
#df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets_wNoise.csv',header=None)
#df_bkg = pd.read_csv('../ConversionTools/JetInfo_qcd_wNoise.csv',header=None)

#scale by pt to get back energy scale
for i in range(1,401):
    df_sig.iloc[:,i] *= df_sig.iloc[:,401]
    df_bkg.iloc[:,i] *= df_bkg.iloc[:,401]

#200k signal, take first 180k for training
df_sig_train = df_sig.iloc[0:179999,:]
df_sig_test  = df_sig.iloc[180000:,:]
#205k background, take first 180k for training
df_bkg_train = df_bkg.iloc[0:179999,:]
df_bkg_test = df_bkg.iloc[180000:,:]
#add together and then shuffle
df_train = pd.concat([df_sig_train,df_bkg_train])
#randomize 100 times so that it's hopefully really random
for i in range(0,100):
    df_train = df_train.sample(frac=1)
df_test = pd.concat([df_sig_test,df_bkg_test])
#randomize 100 times so that it's hopefully really random
#for i in range(0,1):
#    df_test = df_test.sample(frac=1)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_ylabel('Background Rejection')
ax.set_xlabel('Signal Efficiency')
ax.set_ylim([0,1])
ax.set_xlim([0,1])

#run the regression - train first
trainConvTF(400,df_train,df_test)
'''probsSimple,accep,rej = 
ax.scatter(accep,rej,c='blue',label='CNN')


jmeX = 0.6
jmeY = .97
hepX = 0.45
hepY = 0.97

ax.scatter(jmeX,jmeY,color='red',label='SD+tau32')
ax.scatter(hepX,hepY,color='blue',label='HEPTopTagger')

plt.legend(loc='lower left', fontsize = 'small');
fig.savefig("PixelOnly_ROC_CNN.pdf")

#plot probabilities, first merge probabilites and signal back into df
print df_test.head()
df_test = df_test.reset_index()
df_probs = pd.DataFrame(df_test.iloc[:,1]) #get signal bits
print df_probs.head()
probs = pd.DataFrame(probsSimple,columns=['bkg_prob','sig_prob'])
print probs.head()
df_probs['prob']=probs.sig_prob
print df_probs.head()
df_probs_sig = df_probs.loc[(df_probs.loc[:,0]==1)]
df_probs_bkg = df_probs.loc[(df_probs.loc[:,0]==0)]

#now make histogram from each
plt.clf()
ax = fig.add_subplot(111)
ax.set_ylabel('A.U.')
ax.set_xlabel('CNN Probability Output')
#ax.set_ylim([0,1])
#ax.set_xlim([0,1])
h_sig, bins, patches = plt.hist(df_probs_sig['prob'], 50, facecolor='green', alpha=0.75,label='TTbar')
h_bkg, binsbkg, patchesbkg = plt.hist(df_probs_bkg['prob'], 50, facecolor='red', alpha=0.5,label='QCD')
ax.legend(loc=0)
fig.savefig("Discriminant_Output_CNN.pdf")

'''
#now run with pixels and pt of variables
'''print "\n Now running with pT included \n"
alphas = np.logspace(-10,1,11)
fig = plt.figure()
ax = fig.add_subplot(111)
it=0

for alpha in alphas:
    c1 = 1.*( float(it) % 3.)/3.0
    c2 = 1.*( float(it) % 9.)/9.0
    c3 = 1.*( float(it) % 27.)/27.0
    probsSimple,accep,rej = runTensorFlowRegression(401,alpha,df_train,df_test)
    ax.scatter(accep,rej,c=[c1,c2,c3],label='Alpha: %.1E' %alpha)
    it+=1
    
plt.legend(loc='lower left', fontsize = 'small');
fig.savefig("PixelPlusPt_ROC.pdf")
'''
