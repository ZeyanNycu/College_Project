import json
from math import factorial

if __name__ == "__main__":
    with open('config2.json','r') as f:
        config = json.load(f)
    # step1 Read the files from file path
    sequence = open(config['file_path'],'r')
    a,b = config['a'],config['b']
    for idx,s in enumerate(sequence):
        s = s[:-1]
        m = s.count("1")
        
        print(f"case {idx+1}: {s}")
        likelihood = factorial( len(s) )/( factorial(m) * factorial(len(s)-m) )*\
                        ( (m/len(s)) **m )* \
                        ( ((len(s)-m)/len(s))**(len(s)-m) )
        print(f"Beta prior: a = {a} b = {b}")
        print(f"Likelihood: {likelihood}")
        a = a+m
        b = b+len(s)-m
        print(f"Beta posterior: a = {a} b = {b}")