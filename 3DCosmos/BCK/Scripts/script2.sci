
lsta=list;
lsta.add(scalar);
lst1=list;lsta.add(lst1);
lst2=list;lsta.add(lst2);

lsta(0).add(point(9,2,3,4));

lsta.get(1).get(1)=point(1,2,3);



lst=list;
lst.add(1.2);lst.add("aha");lst.add(3.4);
listval1=lst.get(1);
listval2=lst(2);
lst(2)=77.88;
lst.add(point(7,8,9,10));
lst(10)=point(0.1,0.2,0.3,0.4);

lst2=list;
lst2.add(lst);lst2.add(lst);
lst2.add(lst);lst2.add(lst);
lst2.add(lst);lst2.add(lst);
