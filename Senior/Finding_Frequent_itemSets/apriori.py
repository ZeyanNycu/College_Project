"""
Description     : Simple Python implementation of the Apriori Algorithm
Modified from:  https://github.com/asaini/Apriori
Usage:
    $python apriori.py -f DATASET.csv -s minSupport

    $python apriori.py -f DATASET.csv -s 0.15
"""

import sys
from itertools import chain, combinations
from collections import defaultdict
import argparse
import time

file1='statics/file1.txt'
file2='statics/file2.txt'
file3='statics/file3.txt'

def subsets(arr):
    """ Returns non empty subsets of arr"""
    return chain(*[combinations(arr, i + 1) for i, a in enumerate(arr)])

def generated_support(itemSet, transactionList, minSupport, freqSet):
    """calculates the support for items in the itemSet and returns a subset
    of the itemSet each of whose elements satisfies the minimum support"""
    _itemSet = set()
    localSet = defaultdict(int)
    
    for item in itemSet:
        for transaction in transactionList:
            if item.issubset(transaction):
                freqSet[item] += 1
                localSet[item] += 1

    for item, count in localSet.items():
        support = float(count) / len(transactionList)

        if support >= minSupport:
            for i in item:
                _itemSet.add((i,support))

    return _itemSet
def returnItemsWithMinSupport(itemSet, transactionList, minSupport, freqSet):
    """calculates the support for items in the itemSet and returns a subset
    of the itemSet each of whose elements satisfies the minimum support"""
    _itemSet = set()
    localSet = defaultdict(int)
    for item in itemSet:
        ind = 1 
        for transaction in transactionList:
            if item.issubset(transaction):
                freqSet[item] += 1
                localSet[item] += 1
            ind += 1
    for item, count in localSet.items():
        support = float(count) / len(transactionList)
        if support >= minSupport:
            _itemSet.add(item)

    return _itemSet

# 得到itemset的組合
def joinSet(itemSet, length):
    """Join a set with itself and returns the n-element itemsets"""
    return set(
        [i.union(j) for i in itemSet for j in itemSet if len(i.union(j)) == length]
    )

def write_file1(oneCSet,output_path):
    with open(output_path,'w') as f:
        for item, support in sorted(oneCSet, key=lambda x: x[1],reverse=True):
            a = []
            for i in item:
                a.append(i)
            f.write(f'{round(support*100,1)}   {set(a)}\n')

def write_file2(s,oneCSet,output_path):
    with open(output_path,'w') as f:
        f.write(f'{len(oneCSet)}\n')
        f.write(s)

def write_file3(oneCSet,output_path):
    with open(output_path,'w') as f:
        f.write(f"{len(oneCSet)}\n")
        for item, support in sorted(oneCSet, key=lambda x: x[1],reverse=True):
            a = []
            for i in item:
                a.append(i)
            f.write(f'{round(support*100,1)}   {set(a)}\n')
def getItemSetTransactionList(data_iterator):
    transactionList = list()
    itemSet = set()
    for record in data_iterator:
        transaction = frozenset(record)
        transactionList.append(transaction)
        for item in transaction:
            itemSet.add(frozenset([item]))  # Generate 1-itemSets
            
    return itemSet, transactionList


def runApriori(data_iter, minSupport):
    """
    run the apriori algorithm. data_iter is a record iterator
    Return both:
     - items (tuple, support)
    """
    itemSet, transactionList = getItemSetTransactionList(data_iter)
    freqSet = defaultdict(int)
    largeSet = dict()
    # Global dictionary which stores (key=n-itemSets,value=support)
    # which satisfy minSupport
    index = 1
    oneCSet= returnItemsWithMinSupport(itemSet, transactionList, minSupport, freqSet)
    s = (f'{index}\t{len(itemSet)}\t{len(oneCSet)}\n')
    index += 1
    currentLSet = oneCSet
    k = 2
    while currentLSet != set([]):    
        largeSet[k - 1] = currentLSet
        currentLSet = joinSet(currentLSet, k)
        currentCSet= returnItemsWithMinSupport(
            currentLSet, transactionList, minSupport, freqSet
        )
        s += (f'{index}\t{len(currentLSet)}\t{len(currentCSet)}\n')
        index += 1
        currentLSet = currentCSet
        k = k + 1

    def getSupport(item):
        """local function which Returns the support of an item"""
        return float(freqSet[item]) / len(transactionList)

    toRetItems = []
    for key, value in largeSet.items():
        toRetItems.extend([(tuple(sorted(item)), getSupport(item)) for item in value])

    return toRetItems,s
def erase_non_closed(largeSet,currentCSet,freqSet,transactionList):
    if(len(currentCSet)==0):
        return largeSet
    def getSupport(item):
        """local function which Returns the support of an item"""
        return float(freqSet[item]) / len(transactionList)
    ans = set()
    anti_ans = set()
    for i in largeSet:
        sign = True
        for j in currentCSet:
            if i.issubset(j) and getSupport(i)==getSupport(j):
                sign = False
                break
        if(sign):
            ans.add(i)
    return ans 
def runApriori3(data_iter, minSupport):
    """
    run the apriori algorithm. data_iter is a record iterator
    Return both:
     - items (tuple, support)
    """
    itemSet, transactionList = getItemSetTransactionList(data_iter)
    freqSet = defaultdict(int)
    largeSet = dict()
    # Global dictionary which stores (key=n-itemSets,value=support)
    # which satisfy minSupport
    oneCSet= returnItemsWithMinSupport(itemSet, transactionList, minSupport, freqSet)
    currentLSet = oneCSet
    k = 2
    while currentLSet != set([]):    
        largeSet[k - 1] = currentLSet
        currentLSet = joinSet(currentLSet, k)
        currentCSet= returnItemsWithMinSupport(
            currentLSet, transactionList, minSupport, freqSet
        )
        largeSet[k-1] = erase_non_closed(largeSet[k-1],currentCSet,freqSet,transactionList)
        currentLSet = currentCSet
        k = k + 1

    def getSupport(item):
        """local function which Returns the support of an item"""
        return float(freqSet[item]) / len(transactionList)

    toRetItems = []
    for key, value in largeSet.items():
        toRetItems.extend([(tuple(sorted(item)), getSupport(item)) for item in value])

    return toRetItems



def printResults(items):
    """prints the generated itemsets sorted by support """
    for item, support in sorted(items, key=lambda x: x[1]):
        print("item: %s , %.3f" % (str(item), support))


def to_str_results(items):
    """prints the generated itemsets sorted by support"""
    i = []
    for item, support in sorted(items, key=lambda x: x[1]):
        x = "item: %s , %.3f" % (str(item), support)
        i.append(x)
    return i


def dataFromFile(fname):
    """Function which reads from the file and yields a generator"""
    with open(fname, "r") as file_iter:
        for line in file_iter:
            tmp = line
            line = line.strip().split(" ")
            line = line[3:]
            record = frozenset(line)
            yield record


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='ProgramName',
                    description='What the program does',
                    epilog='Text at the bottom of help')

    parser.add_argument('-f', '--inputFile',type=str)
    parser.add_argument('-s', "--minSupport",type=float)
    parser.add_argument('-d', "--destination",type=str)
    args = parser.parse_args()
    import copy
    inFile = None
    inFile = dataFromFile(args.inputFile)
    minSupport = args.minSupport
    print("start to do task1")
    start = time.time()
    items,s = runApriori(inFile, minSupport)
    write_file1(items,f'{args.destination}/step2_task1_{args.inputFile.split(".")[0]}_{args.minSupport}_result1.txt')
    write_file2(s,items,f'{args.destination}/step2_task1_{args.inputFile.split(".")[0]}_{args.minSupport}_result2.txt')
    end = time.time()
    print('運行時間：', str(end - start))
    print("finish task1")
    inFile = dataFromFile(args.inputFile)
    print("start to do task2")
    start = time.time()
    result = runApriori3(inFile, minSupport)
    write_file3(result,f'{args.destination}/step2_task2_{args.inputFile.split(".")[0]}_{args.minSupport}_result1.txt')
    end = time.time()
    print('運行時間：', str(end - start))
    print("finish task2")