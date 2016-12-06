#!/usr/env/python
import pandas as pd
import numpy as np
from RegressionTools import *

#open csv files
df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets.csv',header=None)
df_bkg = pd.read_csv('../ConversionTools/JetInfo_qcd.csv',header=None)
#df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets_wNoise.csv',header=None)
#df_bkg = pd.read_csv('../ConversionTools/JetInfo_qcd_wNoise.csv',header=None)

#add together and then shuffle
df = pd.concat([df_sig,df_bkg])
#randomize 100 times so that it's hopefully really random
for i in range(0,100):
    df = df.sample(frac=1)
#188k total, use the first 150k for training, last 28k for testing
df_train = df.iloc[0:149999,:]
df_test = df.iloc[150000:-1,:]
print df_train.head()
#let's try to run the tensor flow regression using only pixels, which is 400 variables since histogram was 20x20
alphas = np.logspace(-10,1,11)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_ylabel('Background Rejection')
ax.set_xlabel('Signal Efficiency')

it=0

for alpha in alphas:
    c1 = 1.*( float(it) % 3.)/3.0
    c2 = 1.*( float(it) % 9.)/9.0
    c3 = 1.*( float(it) % 27.)/27.0
    probsSimple,accep,rej = runTensorFlowRegression(400,alpha,df_train,df_test)
    ax.scatter(accep,rej,c=[c1,c2,c3],label='Alpha: %.1E' %alpha)
    it+=1

jmeX = 0.6
jmeY = .97
hepX = 0.45
hepY = 0.97

ax.scatter(jmeX,jmeY,color='red',label='SD+tau32')
ax.scatter(hepX,hepY,color='blue',label='HEPTopTagger')

plt.legend(loc='lower left', fontsize = 'small');
fig.savefig("PixelOnly_ROC.pdf")

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
