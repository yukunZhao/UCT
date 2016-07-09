1.How to use the tool?
-dataFile * -thetaFile * -phiFile * -nTopics * -nIters *

2.The arguments?
dataFile: training data, word-pairs of each user.
thetaFile: preceding estimated theta.
phiFile: preceding estimated phi.
nTopics: number of topics.
nIters: times of iteration.

3.Data format?
dataFile: |U_{t}|*|B_{t, u}|
thetaFile: |U_{t}|*|K|
phiFile: |V|*|K|
