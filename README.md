count_naive

a: 1278710
b: 1276801
c: 1277744
d: 1278542
e: 1279703
f: 1278272
g: 1278746
h: 1278055
i: 1278289
j: 1278099
k: 1279885
l: 1275767
m: 1278176
n: 1279137
o: 1277420
p: 1277775
q: 1279875
r: 1278998
s: 1277525
t: 1280015
u: 1278432
v: 1279574
w: 1277378
x: 1277444
y: 1278708
z: 1278138

Time: 7049479 us

------------

count array[512] * 8byte

correct solution

Time: 1732285 us

---

count array[512] * 8byte + 1 producer & 1 consumer

incorrect solution

Time: 1953109 us

--- 

count array[512] * 8byte + 1 producer & n consumer

incorrect solution

Time: 972716 us

--- 