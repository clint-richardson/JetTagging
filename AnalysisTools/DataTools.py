import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def getTrainData(df,nVar):
    ExamplesTrain = df_train.iloc[:,1:nVar+1].as_matrix()
    #now the signal
    ResultsTrain = df_train.iloc[:,0:1].as_matrix()
    return (ExamplesTrain,ResultsTrain)

def getTestData(df,nVar):
    ExamplesTest = df_test.iloc[:,1:nVar+1].as_matrix()
    #now the signal
    ResultsTest = df_test.iloc[:,0:1].as_matrix()
    return (ExamplesTest,ResultsTest)
