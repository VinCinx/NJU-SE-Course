#采用基于用户的协同过滤算法(user-based collaboratIve filtering)
#采用皮尔逊相关系数度量相似度
from math import *
import pandas as pd
import os

#数据初步处理
#相对路径存储
movies = pd.read_csv("..\ml-latest-small\movies.csv",encoding='unicode_escape')
ratings = pd.read_csv("..\ml-latest-small\\ratings.csv")
data = pd.merge(movies, ratings, on='movieId') # 连接两张表，在ratings表里增加电影名称信息
# 合并之后默认按照movieId升序排列
# 改成以用户Id升序排列,去除不需要的列
data=data[['userId','rating','movieId','title']].sort_values(by=['userId'])
#获取当前工作目录
os.getcwd()
data.to_csv('data.csv',index=False)


#统计用户看过的电影，以及对电影的评分信息
data0 = open("data.csv", 'r', encoding='UTF-8')
#字典存储
data = {}
#data:['userId','rating','movieId','title']
for line in data0.readlines()[1:]:
    line = line.strip().split(',')
    # 没有该用户，创建
    if not line[0] in data.keys():
        data[line[0]] = {line[3]: line[1]}
    # 否则添加该电影评分信息
    else:
        data[line[0]][line[3]] = line[1]


#Pearson相关系数计算
#首先会找出两位评论者都曾评分过的电影，利用皮尔逊相关系数公式计算
def pearson(user1, user2):
    user1_data = data[user1]#user1_data里存放用户1评分过的电影，值是评分
    user2_data = data[user2]
    common = {}
    r = 0
    # 找共同电影
    for key in user1_data.keys():
        if key in user2_data.keys():
            common[key] = 1
    if len(common) == 0:
        return 0
    n = len(common)  # 共同电影数目

    #评分和
    sum1 = sum([float(user1_data[movie]) for movie in common])
    sum2 = sum([float(user2_data[movie]) for movie in common])

    #评分平方和
    sum1Sq = sum([pow(float(user1_data[movie]), 2) for movie in common])
    sum2Sq = sum([pow(float(user2_data[movie]), 2) for movie in common])

    #乘积和
    PSum = sum([float(user1_data[it]) * float(user2_data[it]) for it in common])

    #计算相关系数
    num = PSum - (sum1 * sum2 / n)
    den = sqrt((sum1Sq - pow(sum1, 2) / n) * (sum2Sq - pow(sum2, 2) / n))
    if den == 0:
        r=0
    else:
        r = num / den
    return r


#计算某个用户与其他用户的相似度，按照从高到低排序
#找到topN的相关的人
#由于数据量较大，数据集里的人数较多，re的长度会比较大，这里选择top10相关的人
#改变选多少，只要对re进行切片，不过要注意小于len(re)
def topN_similarUsers(userID):
    re = []
    for userid in data.keys():
        if not userid == userID:
            simliar = pearson(userID,userid)
            re.append((userid,simliar))
    re.sort(key=lambda val:val[1])
    #print(len(re))
    return re[:5]



#推荐
def recommend(userID,recNum):
    theSimilarUsers=topN_similarUsers(userID)
    #最大相关的用户，相关度也为0，无法推荐
    #if theSimilarUsers[0][1]==0:
    #    print("由于数据量较小，暂无法推荐")
    #    return 0

    n=len(theSimilarUsers)
    recommendations = {}

    #找n个相关度很高的人，对他们看过的电影的评分进行加权，例如a的相关度为0.9，b的相关度为0.8，a看过一部电影并打分4.5分，b看过同样的电影并打分5分
    #则这部电影预估目标用户的打分为4.5*（0.9/（0.9+0.8））+5*（0.8/（0.9+0.8））
    count=0
    pearsonSum=0
    while count<n:
        topSimUserID = theSimilarUsers[count][0]
        pearsonSum+=pearson(userID,topSimUserID)
        count+=1


    count = 0;
    while count<n:
        topSimUserID = theSimilarUsers[count][0]

        similarMovies = data[topSimUserID]
        for similarMovie in similarMovies.keys():
            if similarMovie not in data[userID].keys():
                if not similarMovie in recommendations.keys():
                    if pearsonSum!=0:
                        recommendations[similarMovie] = float(similarMovies[similarMovie]) * (pearson(userID, topSimUserID) / pearsonSum)
                    else:#最大相关的人相似度为0，置0，此时的推荐是模糊的
                        recommendations[similarMovie] = 0
                else:
                    if pearsonSum!=0:
                        recommendations[similarMovie] += float(similarMovies[similarMovie]) * (pearson(userID, topSimUserID) / pearsonSum)
                    else:
                        recommendations[similarMovie] += 0
        count+=1

    finalRec = sorted(recommendations.items(), key=lambda x: x[1], reverse=True)
    if len(finalRec)>=recNum:
        return finalRec[:recNum]
    else:
        return finalRec



    #下面是直接选择相似度较高的用户看过的电影，去除被推荐者看到的，然后按照评分（相关用户的评分，没有加权）排序
    # judge=1#默认有recNum部可以推荐的电影
    # count=0
    # while(len(recommendations)<recNum ):
    #     #尽管没有找到5部可以推荐的电影，但是没有相似用户了，break
    #     if count>=n:
    #         judge=0
    #         break
    #
    #     topSimUserID = theSimilarUsers[count][0]
    #     # 选择相似度高的用户看过的电影similarMovies
    #     similarMovies = data[topSimUserID]
    #
    #     # 选择该用户未看过的
    #     for similarMovie in similarMovies.keys():
    #         if similarMovie not in data[userID].keys():
    #             recommendations.append((similarMovie, similarMovies[similarMovie]))  # 电影名：评分  尽量推荐相似用户评分高的电影
    #     recommendations.sort(key=lambda val: val[1], reverse=True)  # 按照评分排序
    #     count+=1
    #
    # # 返回评分最高的5部电影
    # if judge==1:
    #     return recommendations[:recNum]
    # else:
    #     return recommendations

finalLst=[]
for i in range(1,611):
    Recommendations = recommend(str(i),5)
    #print(Recommendations)
    lst=[]
    lst.append(i)
    lstMovies=[]
    for j in range(5):
        lstMovies.append(Recommendations[j][0])
    lst.append(lstMovies)
    #print(lst)

    finalLst.append(lst)
    #print(finalLst)

df=pd.DataFrame(columns=['userId','movieId'],data=finalLst)
#print(df)
df = df.set_index('userId')
df.to_csv('movie.csv')









