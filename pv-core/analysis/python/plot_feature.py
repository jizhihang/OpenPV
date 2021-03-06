"""
Plot a color map of features of given phase
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import matplotlib.cm as cm
import PVReadWeights as rw
import PVConversions as conv

def format_coord(x, y):
    col = int(x+0.5)
    row = int(y+0.5)
    if col>=0 and col<numcols and row>=0 and row<numrows:
        z = M[row,col]
        return 'x=%1.4f, y=%1.4f, z=%1.4f'%(x, y, z)
    else:
        return 'x=%1.4f, y=%1.4f'%(x, y)

"""
Show how to modify the coordinate formatter to report the image "z"
value of the nearest pixel given x and y
"""

if len(sys.argv) < 4:
   print "usage: plot_feature orientation ('vertical' or 'horizontal') filename phase filename 2 phase 2"
   sys.exit()

if len(sys.argv) == 4:

   vertical = sys.argv[1]
   phase = int(sys.argv[3])
   if vertical=='horizontal':
      vertical = False
   else:
      vertical = True

   w = rw.PVReadWeights(sys.argv[2])


   # feature for given phase
   #
   f = np.zeros(w.patchSize)
   if vertical:
      f[ range(phase, w.patchSize, w.nxp) ] = 1.0
   else:
      f[ range(phase*w.nxp, w.nxp+phase*w.nxp) ] = 1.0
   print f
   f = w.normalize(f)

   numrows = w.nyGlobal
   numcols = w.nxGlobal

   M = np.zeros(w.numPatches)

   for k in range(len(M)):
      p = w.normalize( w.next_patch() )
      M[k] = np.sum(p * f)

   print "time =", w.time

   # calculate distributions
   #
   numbins = 101
   dist = np.zeros(numbins)
   bins = np.zeros(numbins)
   count = numrows * numcols
   for k in range(numbins): bins[k] = k * 1.0/(numbins-1)
   for k in range(len(M)):
      for b in range(numbins):
         if (M[k] > bins[b]): dist[b] += 1
   dist = dist/count

   # print maximum projection
   #
   maxp = 0
   maxk = 0
   for k in range(len(M)):
      if M[k] > maxp: maxp = M[k]; maxk = k

   kx = conv.kxPos(k, numcols, numrows, w.nf)
   ky = conv.kyPos(k, numcols, numrows, w.nf)
   print "maximum projected value = ", maxp, maxk, kx, ky

   M = M.reshape( (numrows,numcols) )

   # print averaged projection over column
   #
   s = np.zeros(numcols)
   maxs = 0.0
   maxcol = 0
   for col in range(numcols):
      s[col] = np.sum(M[:,col])
      if s[col] > maxs: maxs = s[col]; maxcol = col
   s = s/numrows
   print "(maxcol, maxsum) = (", maxcol, ",", maxs/numrows, ")"

   fig = plt.figure()

   ax = fig.add_subplot(2,1,1)
   ax.set_xlabel('Kx GLOBAL')
   ax.set_ylabel('Ky GLOBAL')
   ax.set_title( 'Feature Projection: phase=%d' %(phase) )
   ax.format_coord = format_coord
   ax.imshow(M, cmap=cm.jet, interpolation='nearest', vmin=0., vmax=1.)

   ax = fig.add_subplot(2,1,2)
   ax.set_xlabel('Feature Strength')
   ax.set_ylabel('Percentage')
   ax.plot(bins, dist, 'o', color='blue')
   #ax.set_ylim(0.0, 0.5)

   plt.show()


if len(sys.argv) == 6:

   vertical = sys.argv[1]
   phaseOn = int(sys.argv[3])
   verticalOff = sys.argv[1]
   phaseOff = int(sys.argv[5])

   if vertical=='horizontal':
      vertical = False
   else:
      vertical = True

   if verticalOff=='horizontal':
      verticalOff = False
   else:
      verticalOff = True

   w = rw.PVReadWeights(sys.argv[2])
   wOff = rw.PVReadWeights(sys.argv[4])

   print phaseOn
   print phaseOff

   # feature for given phase
   #
   f = np.zeros(w.patchSize)
   if vertical:
      f[ range(phaseOn, w.patchSize, w.nxp) ] = 1.0
   else:
      f[ range(phaseOn*w.nxp, w.nxp+phaseOn*w.nxp) ] = 1.0
   print "fOn ", f
   f = w.normalize(f)

   fOff = np.zeros(wOff.patchSize)
   if verticalOff:
      fOff[range(phaseOff, wOff.patchSize, wOff.nxp)] = 1.0
   else:
      fOff[range(phaseOff*wOff.nxp, wOff.nxp+phaseOff*wOff.nxp)] = 1.0
   print "fOff", fOff
   fOff = wOff.normalize(fOff)

   numrows = w.nyGlobal
   numcols = w.nxGlobal
   numrowsOff = wOff.nyGlobal
   numcolsOff = wOff.nxGlobal

   M = np.zeros(w.numPatches)
   MOff = np.zeros(wOff.numPatches)

   for k in range(len(M)):
      p = w.normalize( w.next_patch() )
      pOff = wOff.normalize( wOff.next_patch() )
      M[k] = np.sum(p * f + fOff * pOff) / 2.0

   print "time =", w.time

   # calculate distributions
   #
   numbins = 101
   dist = np.zeros(numbins)
   bins = np.zeros(numbins)
   count = numrows * numcols
   for k in range(numbins): bins[k] = k * 1.0/(numbins-1)
   for k in range(len(M)):
      for b in range(numbins):
         if (M[k] > bins[b]): dist[b] += 1
   dist = dist/count

   # print maximum projection
   #
   maxp = 0
   maxk = 0
   for k in range(len(M)):
      if M[k] > maxp: maxp = M[k]; maxk = k

   kx = conv.kxPos(k, numcols, numrows, w.nf)
   ky = conv.kyPos(k, numcols, numrows, w.nf)
   print "maximum projected value = ", maxp, maxk, kx, ky

   M = M.reshape( (numrows,numcols) )

   # print averaged projection over column
   #
   s = np.zeros(numcols)
   maxs = 0.0
   maxcol = 0
   for col in range(numcols):
      s[col] = np.sum(M[:,col])
      if s[col] > maxs: maxs = s[col]; maxcol = col
   s = s/numrows
   print "(maxcol, maxsum) = (", maxcol, ",", maxs/numrows, ")"

   fig = plt.figure()

   ax = fig.add_subplot(2,1,1)
   ax.set_xlabel('Kx GLOBAL')
   ax.set_ylabel('Ky GLOBAL')
   ax.set_title( 'Feature Projection: phaseOn=%d phaseOff=%d' %(phaseOn, phaseOff) )
   ax.format_coord = format_coord
   ax.imshow(M, cmap=cm.jet, interpolation='nearest', vmin=0., vmax=1.)

   ax = fig.add_subplot(2,1,2)
   ax.set_xlabel('Feature Strength')
   ax.set_ylabel('Percentage')
   ax.plot(bins, dist, 'o', color='blue')
   #ax.set_ylim(0.0, 0.5)

   plt.show()
