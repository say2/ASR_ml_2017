import os


#path="./tst_minimize"
path="./tst"

#dir_path = os.path.abspath(path)
#print dir_path
ff=open("./filename.txt",'wb')
for (p,d,f) in os.walk(path):
    #print path,dir,files
    #print p,d,f
    for fname in f:
        fullfname = p + "/" + fname
        if 'DS_Store' in fname:
            continue
        ff.write(fullfname+'\n')
ff.close()

