-- ProtoScale 3 Bus Model

import Power

create Bus:
  b0, b1, b4;

create Source:
  s0{f=5}, s1{f=2};

create Generator:
  g0, g1;

create Load:
  l0{f=7}
  

create Line [ym, ya]: 
  l0 [0.0623, 1.2543] b0 b1
  l1 [0.2295, 1.3331] b0 b4
  l2 [0.1830, 1.2543] b1 b4

