import sys 
import os
sys.path.append('lib')
import lib1 as L
L.hello()

sys.path.append('lib/math')
import mod as M
print('add(2,3)=', M.add(2,3))
