import urllib.request
import os
data_dir = os.path.join(os.path.dirname(os.path.realpath('__file__')), "data")
if not os.path.exists(data_dir):
    os.makedirs(data_dir)

filename = os.path.join(data_dir, "rick.mp3")
if not os.path.exists(filename):
    raise "Please download data/rick.mp3 first"