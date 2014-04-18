-- ProtoScale 3 Bus Model

import Power

create Bus {vm=1, va=0}:
  b0, b1, b4

create Line [ym, ya]: 
  l0 [0.0623, 1.2543] b0 b1,
  l1 [0.2295, 1.3331] b0 b4,
  l2 [0.1830, 1.2543] b1 b4,

