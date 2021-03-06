"""
Make a histogram of normally distributed random numbers and plot the
analytic PDF over it
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import PVReadWeights as rw

if len(sys.argv) < 2:
   print "usage: plot_weight_histogram filename1 filename2"
   exit()


if len(sys.argv) == 2:

   w = rw.PVReadWeights(sys.argv[1])
   h = w.histogram()

   low = 0
   high = 0

   for i in range(len(h)):
      if i < 126 and h[i] > 200: low += h[i]
      if i > 126 and h[i] > 200: high += h[i]

   print "low==", low, "high==", high, "total==", np.add.reduce(h)


   w_split_val = 255/2.
   if len(sys.argv) >= 3:
      w_split_val = float(sys.argv[2])

   ch = w.clique_histogram(w_split_val)
   print 'total =', sum(h)


   fig = plt.figure()
   ax = fig.add_subplot(211)

   ax.plot(np.arange(len(h)), h, '-o', color='b', linewidth=5.0)

   ax.set_xlabel('WEIGHT BINS')
   ax.set_ylabel('COUNT')
   ax.set_title('Weight Histogram')
   ax.set_xlim(0, 256)
   ax.grid(True)


   ax = fig.add_subplot(212)

   ax.plot(np.arange(len(ch)), ch, '-o', color='b',  linewidth=5.0)

   ax.set_xlabel('CLIQUE BINS')
   ax.set_ylabel('COUNT')
   ax.set_title('Clique Histogram')
   ax.set_xlim(0, 1+w.patchSize)
   ax.grid(True)

   plt.show()



else:
   w = rw.PVReadWeights(sys.argv[1])
   wOff = rw.PVReadWeights(sys.argv[2])

   h = w.histogram()
   hOff = wOff.histogram()

   low = 0
   high = 0
   lowOff = 0
   highOff = 0

   for i in range(len(h)):
      if i < 126 and h[i] > 200: low += h[i]
      if i > 126 and h[i] > 200: high += h[i]
   for i in range(len(hOff)):
      if i < 126 and hOff[i] > 200: lowOff += hOff[i]
      if i > 126 and hOff[i] > 200: highOff += hOff[i]

   print "On Weights: low==", low, "high==", high, "total==", np.add.reduce(h)
   print "Off Weights: low ==", lowOff, "high==", highOff, "total==", np.add.reduce(hOff)


   w_split_val = 255/2.
   if len(sys.argv) >= 4:
      w_split_val = float(sys.argv[3])


   chOn = w.clique_histogram(w_split_val)
   chOff = wOff.clique_histogram(w_split_val)
   print 'Clique On total =', sum(chOn)
   print 'Clique Off total =', sum(chOff)




   fig = plt.figure()


   ax = fig.add_subplot(221)
   ax.plot(np.arange(len(h)), h, '-o', color='b', linewidth=5.0)
   #ax.set_xlabel('WEIGHT BINS')
   ax.set_ylabel('ON WEIGHTS', fontsize = 'large')
   ax.set_title('Weight Histograms')
   ax.set_xlim(0, 256)


   ax = fig.add_subplot(223)
   ax.plot(np.arange(len(hOff)), hOff, '-o', color='b', linewidth=5.0)
   ax.set_xlabel('WEIGHT BINS')
   ax.set_ylabel('OFF WEIGHTS', fontsize = 'large')
   #ax.set_title('Weight Histogram')
   ax.set_xlim(0, 256)


   ax = fig.add_subplot(222)

   ax.plot(np.arange(len(chOn)), chOn, '-o', color='b', linewidth=5.0)

   #ax.set_xlabel('CLIQUE BINS')
   ax.set_ylabel('COUNT')
   ax.set_title('Clique Histograms')
   ax.set_xlim(0, 1+w.patchSize)
   ax.grid(True)



   ax = fig.add_subplot(224)

   ax.plot(np.arange(len(chOff)), chOff, '-o', color='b', linewidth=5.0)

   ax.set_xlabel('CLIQUE BINS')
   ax.set_ylabel('COUNT')
   #ax.set_title('Off Clique Histogram')
   ax.set_xlim(0, 1+w.patchSize)
   ax.grid(True)


   plt.show()
