```
 ____   ___ ___ _   _ _   _ 
/ ___| / _ \_ _| \ | | \ | |
\___ \| | | | ||  \| |  \| |
 ___) | |_| | || |\  | |\  |
|____/ \___/___|_| \_|_| \_|

```
SOINN is a blazing fast unsupervised clustering algorithm.
This is the repo for python package soinn.


## Install
`
pip install soinn
`

## Build by hand
`
python -m build
`



## API

`learn(data: object = None, dead_age: int = 100, lambda: int = 100, noise: float = 0.5, num_layer: int = 1) -> object`

**input** : numpy array of shape (n, dim)

**dead_age** : the dead age of edges during the learning process, default 100

**lamda** : the number of iterations before removing noise, default 100

**noise** : the ratio of noise in input data, default 0.5

**num_layer** : the number of soinn layer, default 1

**return** : return a numpy array contaning all learned cendrioids, shape (n1, dim) where
n1 represents the number of cendrioids.



## Run demo
Here is a simple code using SOINN to learn cendroids of the input data



```python
from scipy.io import loadmat

import matplotlib.pyplot as plt

import soinn



data = loadmat("train.mat")['train']

print('Load data with shape', data.shape)



ax=plt.subplot(121)

ax.set_title('origin data')

plt.plot(data[:,0], data[:,1], '.')



clus = soinn.learn(data)

print('soinn learned clusters with shape', clus.shape)

ax=plt.subplot(122)

ax.set_title('learned cendroids')

plt.plot(clus[:,0], clus[:,1], '.')



plt.show()
```

You can run it by:

`
cd demo && python demo.py
`

You can get the following result
![image](https://github.com/uestc-lfs/soinn/blob/master/demo/result.png)

