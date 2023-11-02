import libavpy as av
import numpy as np

a = np.array(av.Frame(1920, 1280, av.PixelFormat.GRAY8))
b = av.Frame.from_numpy(a.T, av.PixelFormat.GRAY8, copy=False)