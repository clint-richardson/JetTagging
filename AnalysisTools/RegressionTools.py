from sklearn.neural_network import MLPClassifier
from sklearn.linear_model import SGDClassifier
import tensorflow as tf

from DataTools import *

def runSciKitRegressionL2(nVar,alpha,df_train,df_test):
    X_train, y_train = getTrainData(df_train,nVar)
    X_test, y_test = getTestData(df_test,nVar)
    clf = SGDClassifier(loss="log", penalty="l2",alpha=alpha)
    clf.fit(X_train,y_train)
    predictions = clf.predict(X_test)
    print 'Accuracy on test data with alpha %.2E : %.3f' %(alpha,clf.score(X_test,y_test))
    probs = clf.predict_proba(X_test)
    #print probs
    #get signal acceptance and background rejection
    thresholds = np.arange(0,1,.01)
    Acceptance = []
    BkgRejection = []
    for thresh in thresholds:
        it=0
        nPredSig=0.0
        nPredBkg=0.0
        nTotSig=0.0
        nTotBkg=0.0
        for prob in probs:
            if prob[1]>thresh:
                predSig=True
            else:
                predSig=False
            if y_test[it][0]:
                Sig = True
            else:
                Sig = False
            if Sig:
                if predSig:
                    nPredSig+=1
                nTotSig+=1
            else:
                if not predSig:
                    nPredBkg+=1
                nTotBkg+=1
            it+=1
        Acceptance.append(nPredSig/nTotSig)
        BkgRejection.append(nPredBkg/nTotBkg)
    return (Acceptance,BkgRejection)


def runTensorFlowRegression(nVar,alpha,df_train,df_test):

    #make data array placeholder for just first 8 simple features
    x = tf.placeholder(tf.float32,[None,nVar])
    #make weights and bias
    W = tf.Variable(tf.zeros([nVar,2])) #we will make y 'onehot' 0 bit is bkg, 1 bit is signal
    b = tf.Variable(tf.zeros([2]))

    #make 'answer variable'
    y = tf.nn.softmax(tf.matmul(x, W) + b)
    #placeholder for correct answer
    y_ = tf.placeholder(tf.float32, [None, 2])
    #cross entropy
    cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(y, y_)+alpha*tf.nn.l2_loss(W))

    #define training step
    train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)
    #initialize variables
    init = tf.initialize_all_variables()
    #setup session
    sess = tf.Session()
    sess.run(init)

    #ok now everything is setup for tensorflow, but we need the data in a useful form
    #first let's get the variables
    Var_train,Sig_train_bit1 = getTrainData(df_train,nVar)#df_train.iloc[:,1:nVar+1].as_matrix()
    #print Var_train,Sig_train_bit1
    #now the signal
    #Sig_train_bit1 = df_train.iloc[:,0:1].as_matrix()
    Sig_train_bit0 = Sig_train_bit1.copy()
    Sig_train_bit0 = 1 - Sig_train_bit0
    Sig_train = np.column_stack((Sig_train_bit0,Sig_train_bit1))

    #now run with batches of 100 data points
    for i in range(0,1500):
        start = i*100
        end = (i+1)*100-1
        batch_x = Var_train[start:end]
        batch_y = Sig_train[start:end]
        sess.run(train_step, feed_dict={x: batch_x, y_: batch_y})
    
    
    #now test accuracy
    correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    #setup test data
    Var_test,Sig_test_bit1  = getTestData(df_test,nVar)
    #print Var_test,Sig_test_bit1
    Sig_test_bit0 = Sig_test_bit1.copy()
    Sig_test_bit0 = 1 - Sig_test_bit0
    Sig_test = np.column_stack((Sig_test_bit0,Sig_test_bit1))
    print"Accuracy for alpha %.1E : %.3f" %(alpha,sess.run(accuracy, feed_dict={x: Var_test, y_: Sig_test}))
    
    #get the weights
    weights = W.eval(session=sess)
    #get probabilities assigned (i.e. evaluate y on test data)
    probs = y.eval(feed_dict = {x: Var_test}, session = sess)
    #print probs
    #now let's get the signal efficiency and background rejection on the test data
    Acceptance = []
    Rejection = []
    theshes = np.arange(0,1,0.01)
    for thresh in theshes:
        it=0
        nBkg = 0.0
        nBkgCor = 0.0
        nSig = 0.0
        nSigCor = 0.0
        for prob in probs:
            if prob[0] > thresh:
                sig=False
            else:
                sig=True
            if Sig_test_bit1[it][0]: #actual signal
                nSig+=1
                if sig:
                    nSigCor+=1
            else: #actual background
                nBkg+=1
                if not sig:
                    nBkgCor+=1
                
            it+=1
        #now append signal efficiency and bakcground rejection
        if nSig==0:
            Acceptance.append(0)
        else:
            Acceptance.append(nSigCor/nSig)
        if nBkg==0:
            Rejection.append(0)
        else:
            Rejection.append(nBkgCor/nBkg)
    #print 'first 100 probabilities'
    #print probs[0:100]
    return (probs,Acceptance,Rejection)
