mt=matrix(4,4);

for i=1 to 4 do
   for j=1 to 4 do
      mt(i,j)=i*j+i;

mt3=8*unitmatrix(4);

mt2=3*mt.transpose;
vl=mt(2,2);

mtt=mt2-2*mt3;