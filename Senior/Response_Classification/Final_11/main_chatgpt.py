import json
import openai
import time

e = "Paragrph: 这不是打电话或者写电子邮件就能解决的问题，我们还是见面再说吧。我在公司旁边那个咖啡店等你。\n8156. 问题终于解决了。\nA. 正确\nB. 错误\nParagrph: 每天早上我到办公室后的第一件事就是打开电脑，查看电子邮件。然后根据这些邮件来决定这一天我该先做什么后做什么。\n8157. 我每天：\nA. 工作特别忙\nB. 早上安排工作\nC. 很早到办公室\n\
\n回答我ABCD即可,不用解釋"

p="\n8156. B\n8157. B"

e1="Paragrph: 这不是打电话或者写电子邮件就能解决的问题，我们还是见面再说吧。我在公司旁边那个咖啡店等你。\n8156. 问题终于解决了。\nA. 正确\nB. 错误\n\
\n回答我ABCD即可,不用解釋"
p1="\n8156. B"

e2="Paragrph: 一座小山的山顶上有个“山顶美食”，美食店的房子是竹棚搭建的，整个店面看起来简易，但建造得还挺结实，也很合理。厨房有独立的几个制作部，点心档、蒸档、明档、刺身档等，有一些规模。那里的拿手菜是碎仔骨。据说这骨头既有陈皮的果香味，又有陈醋的微酸，很开胃，肉吃起来软硬适中，又有嚼头，火候恰到好处。\
山顶美食虽是大排档的风格，却很注重营造企业文化，经常举行一些活动，激发员工的创新意识。“金碧辉煌”就是这样得来的，不少顾客品尝后都赞不绝口。“金碧辉煌”其实是将日本豆腐油炸后的一道菜，但创新者的聪明之处是，在豆腐的底部放上了一些酸菜。外香微脆的炸豆腐，咸味，加上开胃的酸菜，酸中有甜，还添了点辣，五味中占了四味!\n\
8023. 山顶美食店的店铺是用什么材料建造的?\n\
A. 石头\n\
B. 砖块\n\
C. 竹子\n\
D. 木头\n\
8024. 这家店最有名的菜是：\n\
A. 碎仔骨\n\
B. 水饺\n\
C. 蛋糕\n\
D. 金碧辉煌\n\
8025. 这家店是什么样的风格?\n\
A. 高档美食\n\
B. 大排档\n\
C. 白领餐厅\n\
D. 家常饭馆\n\
\n回答我ABCD即可,不用解釋"

p2="8023. C\n8024. A\n8025. B"


f_train = open('train.json')
f_val = open('val.json')
f_test = open('test.json')
data_train = json.load(f_train)
data_val = json.load(f_val)
data_test =json.load(f_test)

print(len(data_train))
print(len(data_val))
print(len(data_test))
temp = ["A","B","C","D"]

def preprocess_t (data,k):
    output =""
    p = 0
    i = k
    for c in range(1):
        if i==len(data):
            break
        statement = f"Paragrph: {data[i]['paragraph']}\n"
        combine=""
        #print(statement)
        for j in range(len(data[i]['questions'])):
            quest = f"{data[i]['questions'][j]['id']}. {data[i]['questions'][j]['question']}\n"
            choice =""
            for k in range (len(data[i]['questions'][j]['choice'])):
                choice +=f"{temp[k]}. {data[i]['questions'][j]['choice'][k]}\n"
            #answer.append(temp[data[i]['questions'][j]['choice'].index(data[i]['questions'][j]['answer'])])
            combine += quest
            combine += choice
            p+=1
        i+=1
        output += statement
        output += combine
    #print(answer)
    return output , p

def preprocess (data):
    output =""
    i = 0
    p = 0
    answer=[]
    while(1):
        if(p>=20):
            break
        statement = f"Paragrph: {data[i]['paragraph']}\n"
        combine=""
        #print(statement)
        for j in range(len(data[i]['questions'])):
            quest = f"{data[i]['questions'][j]['id']}. {data[i]['questions'][j]['question']}\n"
            choice =""
            for k in range (len(data[i]['questions'][j]['choice'])):
                choice +=f"{temp[k]}. {data[i]['questions'][j]['choice'][k]}\n"
            answer.append(temp[data[i]['questions'][j]['choice'].index(data[i]['questions'][j]['answer'])])
            combine += quest
            combine += choice
            p+=1
        i+=1
        output += statement
        output += combine
    #print(answer)
    return output , answer
        #print(data[i])

#t = preprocess_t(data_test)
a,b = preprocess(data_train)
ask="\n將答案用一個陣列來裝,陣列長度等於題目數量,output example:[0,1,1,0,....]\n"
ask2="\n回答我0、1即可,不用解釋\n"
ask1="\nONLY NEED TO  OUTPUT A LIST! output a list contains all the answer ex:[1,0,1,0,....]\n"

prompt =a+ask2

api_key = 'X'
'''
openai.api_key = api_key
response = openai.ChatCompletion.create(
      model="gpt-3.5-turbo-1106",  
      messages=[{"role": "user", "content": prompt}
                ]
    )
print(b,"\n")
print(response['choices'][0]['message']['content'])
'''
m=["gpt-3.5-turbo-1106","gpt-3.5-turbo-0613","gpt-3.5-turbo-0301","gpt-3.5-turbo-16k-0613","gpt-3.5-turbo-16k"]
total=[]
should = 0 
l1 = 1
i=358
while(1):
    if i == len(data_test):
        break
    #if i%1 == 0:
    print(i)
    t , count= preprocess_t(data_test,i)
    should += count
    prompt1 = t +ask2
    print(prompt1)
    #l=i/3
    
    
    openai.api_key = api_key
    response = openai.ChatCompletion.create(
        model=m[int(l1%5)],  
        messages=[
            {"role": "user", "content": e1},
            {"role": "assistant", "content": p1},
            {"role": "user", "content": e2},
            {"role": "assistant", "content": p2},
            {"role": "user", "content": prompt1}
                    ]
        )
    #print(response)
    str1=response['choices'][0]['message']['content']
    k = open("content.txt",'a')
    k.write(str1)
    k.close

    arr=str1.split('. ')
    if(len(arr)==count+1):
        for j in range(1,len(arr)):
            if(arr[j][0] in temp):
                total.append(temp.index(arr[j][0]))
    if should != len(total):
        print("re dooooooooooooooooooooooooooooooooooooooooooo")
        should -= count
        i-=1
    else:
        f = open("fail.txt", 'a')
        pp = f"paragraph{i}: {count}\n"
        f.write(pp)
        f.write(' '.join(map(str, total[should-count:])))
        f.write("\n")
        f.close()
    #print(i,"\n",should-count,"\n",count,"\n")
    l1+=1
    i+=1
    time.sleep(4.1)
        


f = open("success.txt", 'a')
f.write(' '.join(map(str, total)))
f.close()
print("success")