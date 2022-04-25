from sklearn.datasets import load_iris # 导入鸢尾花数据集
from sklearn import tree
from sklearn.model_selection import train_test_split

import graphviz

# 加载iris数据集、分割
def load_data():
    iris = load_iris()
    X_train,X_test,Y_train,Y_test = train_test_split(iris.data,iris.target,test_size=0.4) #从数据集中随机划分训练集和测试集，这里按照训练集：测试集=6：4进行划分
    return X_train, X_test, Y_train, Y_test

# 定义主函数
def main():
    # 分割数据集
    x_train, x_test, y_train, y_test = load_data()

    clf = tree.DecisionTreeClassifier(criterion="entropy") # 设置决策树分类器
    clf.fit(x_train, y_train) # 训练模型
    score = clf.score(x_test, y_test) # 模型评估
    print("\n模型测试集准确率为：", score)
    
    iris = load_iris()
    feature_names = iris.feature_names
    target_names = iris.target_names
    clf_dot = tree.export_graphviz(clf,
                                   out_file= None,
                                   feature_names= feature_names,
                                   class_names= target_names,
                                   filled= True,
                                   rounded= True) # 绘制决策树模型
    
    graph = graphviz.Source(clf_dot, 
    						filename= "iris_decisionTree.gv", 
    						format= "png")
    graph.view()

    
if __name__ == "__main__":
    main()

