import sys

check={"0","1","2","3","4","5","6","7","8","9"}
def validateBoruvka(filename):
    mismatchedEdges = set()
    mstEdgesSeq = set()
    mstEdgesPar = set()
    isSequential = True
    f = open(filename)

    parEdgeCount = 0
    for line in f.readlines():
        
        line = line.strip()
        print(line)
        if line == "end sequential":
            isSequential = False
            continue
        if "time" in line:
            continue
        if line[0] not in check:
            continue
        elems = line.split(',')
        e = (int(elems[0]), int(elems[1]))
        eRev = (int(elems[1]), int(elems[0]))
        if isSequential:
            mstEdgesSeq.add(e)
            mstEdgesSeq.add(eRev)
        else:
            mstEdgesPar.add(e)
            mstEdgesPar.add(eRev)         
    f.close()

    symmDiff = mstEdgesSeq.symmetric_difference(mstEdgesPar)
    print (symmDiff)
    return (len(symmDiff) == 0)

filename = sys.argv[1]
print (validateBoruvka(filename))
