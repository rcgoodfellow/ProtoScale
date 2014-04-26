`ProtoScale 3 Bus Model`

import Power;

create Bus b0, b1, b4;

create Source{p} s0{5}, s1{2};

create Generator g0, g1;

create Load{fp} l0{7};

create Line{y, ya}
  l0{0.0623, 1.2543},
  l1{0.2295, 1.3331},
  l2{0.1830, 1.2543};

connect b0 b1 l0,
        b0 b4 l1,
        b1 b4 b4;

