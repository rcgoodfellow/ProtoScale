`ProtoScale 3 Bus Model`

model ThreeBus:

import Power;

create Bus b0, b1, b3;

create Source{p} s0{5}, s1{2};

create Generator g0, g1;

create Load{fp} ld0{7};

create Line{y, ya}
  l0{0.0623, 1.2543},
  l1{0.2295, 1.3331},
  l2{0.1830, 1.2543},
  zl{0, 0};

`bus interconnections`
connect symmetric 
  b0 bus b1 l0,
  b0 bus b3 l1,
  b1 bus b3 l2;

`connect in generators` 
connect
  b0 gen g0 zl,
  b1 gen g1 zl;

`connect in sources`
connect 
  g0 source s0 zl,
  g1 source s1 zl;

:: `end of model ThreeBus`

