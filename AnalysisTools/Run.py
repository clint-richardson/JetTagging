#!/usr/env/python
import pandas as pd
from RegressionTools import *

#open csv files
df_sig = pd.read_csv('../ConversionTools/JetInfo_TopJets.csv')
df_bkg = pd.read_csv('../ConversionTools/JetInfo_QCD.csv')

#add together and then shuffle
df = pd.concat([df_sig,df_bkg])
#randomize three times so that it's hopefully really random
df = df.sample(frac=1)
df = df.sample(frac=1)
df = df.sample(frac=1)

#use the first 18k for training, last 5k for testing
df_train = df.iloc[0:17999,:]
df_test = df.iloc[18000:-1,:]
#let's try to run the tensor flow regression using only pixels, which is 400 variables since histogram was 20x20
runTensorFlowRegression(400,0.00001,df_train,df_test)
