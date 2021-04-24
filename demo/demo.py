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
