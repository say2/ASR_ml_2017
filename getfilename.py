import os


path="./tst_minimize"

dir_path = os.path.abspath(path)
print dir_path
ff=open("./filename.txt",'wb')
for (p,d,f) in os.walk(dir_path):
    #print path,dir,files
    for fname in f:
        fullfname = p + "/" + fname
        ff.write(fullfname+'\n')
ff.close()

