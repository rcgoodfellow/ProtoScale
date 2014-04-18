-- Metamodel three take one

import Three

create Alpha {a=1, b=1, c=1, v=1} a
create Beta {a=1, b=1, v=1} b
create Gamma {a=1, b=c, c=1, d=1, v=1} g

create Connector [x]
  c0 [3] a b
  c1 [4] b g
  c2 [7] g a

