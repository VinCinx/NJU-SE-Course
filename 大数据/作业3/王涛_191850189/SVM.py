from sklearn.datasets import load_iris #导入鸢尾花数据集
from sklearn.model_selection import train_test_split

# 加载iris数据集、分割
def load_data():
    iris = load_iris()
    X_train,X_test,Y_train,Y_test = train_test_split(iris.data,iris.target,test_size=0.4) #从数据集中随机划分训练集和测试集，这里按照训练集：测试集=6：4进行划分
    return X_train, X_test, Y_train, Y_test




from sklearn import svm

def test_SVC(X_train,X_test,Y_train,Y_test):
    svm_classifier = svm.SVC(C=1.0, kernel='rbf', decision_function_shape='ovr', gamma=0.01) #构建
    svm_classifier.fit(X_train, Y_train) #训练
    # 评估
    print("训练集准确率:", svm_classifier.score(X_train, Y_train))
    print("测试集准确率:", svm_classifier.score(X_test, Y_test))
 
if __name__=="__main__":
    X_train,X_test,Y_train,Y_test=load_data()     
    test_SVC(X_train,X_test,Y_train,Y_test)
    