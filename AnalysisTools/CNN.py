import tensorflow as tf

from DataTools import *

def weight_variable(shape):
    initial = tf.truncated_normal(shape, stddev=0.1)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.1, shape=shape)
    return tf.Variable(initial)

def conv2d(x, W):
  return tf.nn.conv2d(x, W, strides=[1, 1, 1, 1], padding='SAME')

def max_pool_2x2(x):
  return tf.nn.max_pool(x, ksize=[1, 2, 2, 1],
                        strides=[1, 2, 2, 1], padding='SAME')

def runConvTF(nVar,df_train,df_test):
    #define variables for inputs and output classification
    x_ = tf.placeholder(tf.float32, shape=[None, nVar])
    y_ = tf.placeholder(tf.float32, shape=[None, 2])
    #make weights and bias
    W = tf.Variable(tf.zeros([nVar,2])) #we will make y 'onehot' 0 bit is bkg, 1 bit is signal
    b = tf.Variable(tf.zeros([2]))


    #get x_ as 'image' and convolve with first layer - 4x4 features so that they are of order a subjet
    W_conv1 = weight_variable([4, 4, 1, 32])
    b_conv1 = bias_variable([32])
    #20x20 because that is binning used in histogram creation
    x_image = tf.reshape(x_, [-1,20,20,1])
    h_conv1 = tf.nn.relu(conv2d(x_image, W_conv1) + b_conv1)
    h_pool1 = max_pool_2x2(h_conv1)
    #now add second layer
    W_conv2 = weight_variable([4, 4, 32, 64])
    b_conv2 = bias_variable([64])

    h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2) + b_conv2)
    h_pool2 = max_pool_2x2(h_conv2)

    #now add the densely connected layer - images are 5x5 now
    W_fc1 = weight_variable([5 * 5 * 64, 1024])
    b_fc1 = bias_variable([1024])
    
    h_pool2_flat = tf.reshape(h_pool2, [-1, 5*5*64])
    h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, W_fc1) + b_fc1)
    #add dropout
    keep_prob = tf.placeholder(tf.float32)
    h_fc1_drop = tf.nn.dropout(h_fc1, keep_prob)

    #now readout the values
    W_fc2 = weight_variable([1024, 2])
    b_fc2 = bias_variable([2])

    y_conv = tf.nn.softmax(tf.matmul(h_fc1_drop, W_fc2) + b_fc2)

    #now define training
    cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(y_conv, y_))
    train_step = tf.train.AdamOptimizer(1e-4).minimize(cross_entropy)
    correct_prediction = tf.equal(tf.argmax(y_conv,1), tf.argmax(y_,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    #make session and initialize variables
    sess = tf.Session()
    init = tf.initialize_all_variables()
    sess.run(init)
    #now get data for tensorflow to use
    Var_train,Sig_train_bit1 = getTrainData(df_train,nVar)
    Sig_train_bit0 = Sig_train_bit1.copy()
    Sig_train_bit0 = 1 - Sig_train_bit0
    Sig_train = np.column_stack((Sig_train_bit0,Sig_train_bit1))

    for i in range(0,1500):
        start = i*100
        end = (i+1)*100-1
        batch_x = Var_train[start:end]
        batch_y = Sig_train[start:end]
        if i%100 == 0:
            train_accuracy = sess.run(accuracy,feed_dict={x_:batch_x, y_: batch_y, keep_prob: 1.0})
            print 'training accuracy at step %i is %.3f' %(i,train_accuracy)
        sess.run(train_step,feed_dict={x_: batch_x, y_: batch_y, keep_prob: 0.5})


    #setup for testing
    #setup test data
    Var_test,Sig_test_bit1  = getTestData(df_test,nVar)
    #print Var_test,Sig_test_bit1
    Sig_test_bit0 = Sig_test_bit1.copy()
    Sig_test_bit0 = 1 - Sig_test_bit0
    Sig_test = np.column_stack((Sig_test_bit0,Sig_test_bit1))

    #now get the probabilities
    probs = y_conv.eval(feed_dict={x_: Var_test,keep_prob : 1.0}, session = sess)
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
