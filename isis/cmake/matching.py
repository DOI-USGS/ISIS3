# Deal with annoying numpy warning
import warnings
warnings.filterwarnings("ignore")

from shapely import wkt
import glob
import os

import argparse

import autocnet
from autocnet import CandidateGraph
from autocnet.graph.edge import Edge
from autocnet.matcher import suppression_funcs

from shapely.geometry import Point
from PIL import Image, ImageDraw

from scipy.stats.mstats import zscore
from scipy.stats import gaussian_kde

import pandas as pd

import geopandas as gpd
from shapely.geometry import Polygon, mapping, box

from osgeo import gdal
from osgeo import osr
import numpy as np
from plio.io.io_gdal import GeoDataset

import subprocess
from subprocess import Popen, PIPE

from pysis.isis import spiceinit, footprintinit, jigsaw, pointreg, cam2map
from pysis.exceptions import ProcessError

from plio.date import marstime
from collections import OrderedDict

import yaml

from functools import partial
import utils


record = OrderedDict({
    'file_id1' : '',
    'file_id2' : '',
    'file_path1' : '',
    'file_path2': '',
    'localtime1' : 0,
    'localtime2' : 0,
    'solarlon1' : 0,
    'solarlon2' : 0,
    'mars_year1' : 0,
    'mars_year2' : 0,
    'delta_time' : 0,
    'residual_min' : 0,
    'residual_max' : 0,
    'incident_angle1' : 0,
    'incident_angle2' : 0,
    'stddev1' : 0,
    'stddev2' : 0,
    'avg1' : 0,
    'avg2' : 0,
    'min1' : 0,
    'min2' : 0,
    'max1' : 0,
    'max2' : 0,
    'diff_avg' : 0,
    'diff_stddev' : 0,
})



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('id1', action='store', help='Image ID for image one')
    parser.add_argument('id2', action='store', help='Image ID for image two')
    parser.add_argument('-c', '--config', action='store', help='path to config file', default='./config.yml')
    parser.add_argument('-v', '--verbose', action='store_true', help='Activates verbose output', default=False)
    parser.add_argument('-g', '--graph', action='store_true', help='Display graphs of the data as is goes through.', default=False)
    args = parser.parse_args().__dict__

    with open(args['config'], 'r') as ymlfile:
        cfg = yaml.load(ymlfile)

    if args['verbose']:
        def verboseprint(*args):
            # Print each argument separately so caller doesn't need to
            # stuff everything to be printed into a single string
            import pprint
            pp = pprint.PrettyPrinter(indent=2)
            for arg in args:
               pp.pprint(arg),
            print()
    else:
        verboseprint = lambda *a: None      # do-nothing function

    verboseprint('ARGS:', args)
    verboseprint('CONFIG:', cfg)
    # patch in davinci_bin onto run_davinci to prevent contstantly having to pass it in
    run_davinci = partial(utils.run_davinci, root_dir=config['inpath'])

    files = [os.path.abspath('{}.lev1.cub'.format(id1)), os.path.abspath('{}.lev1.cub'.format(id2))]
    img1, img2 = files
    cubelis = '{}_{}.lis'.format(id1, id2)

    record['id1'] = args['id1']
    record['id2'] = args['id2']
    record['img1_path'] = img1
    record['img2_path'] = img2

    #############################################################

    cg = CandidateGraph.from_filelist(files)

    # The range of DN values over the data is small, so the threshold for differentiating interesting features must be small.
    cg.extract_features(extractor_parameters={'contrastThreshold':0.0000000001})
    cg.match()

    e = cg.edge[0][1]
    e.symmetry_check()
    e.ratio_check(clean_keys=['symmetry'])

    cg.compute_fundamental_matrices(clean_keys=['symmetry', 'ratio'], reproj_threshold=1, mle_reproj_threshold=0.2)
    cg.suppress(clean_keys=['fundamental'], suppression_func=suppression_funcs.distance, k=20, min_radius=10)

    cnet = '{}_{}'.format(filid1, filid2)
    cnet_file = '{}.net'.format(cnet)
    cg.create_control_network(clean_keys=['fundamental', 'suppression'])
    cg.to_isis(cnet)

    intersection = e.destination.footprint.intersection(e.source.footprint)

    pointreg_params_pass1 = {
        'from_' : cubelis,
        'cnet' : cnet_file,
        'onet' : cnet_file,
        'deffile': 'pointreg_P71x151_S131x231.def'
    }

    pointreg_params_pass2 = {
        'from_' : cubelis,
        'cnet' : cnet_file,
        'onet' : cnet_file,
        'deffile': 'pointreg_P51x51_S65x65.def'
    }

    try:
        pointreg(**pointreg_params_pass1)
        pointreg(**pointreg_params_pass2)
    except ProcessError as e:
        print('Pointreg Error')
        print("STDOUT:", e.stdout.decode('utf-8'))
        print("STDERR:", e.stderr.decode('utf-8'))

    bundle_parameters = {
        'from_' : cubelis,
        'cnet' : cnet_file,
        'onet' : cnet_file,
        'radius' : 'yes',
        'update' : 'yes',
        'errorpropagation' : 'no',
        'outlier_rejection' : 'no',
        'sigma0' : '1.0e-10',
        'maxits' : 10,
        'camsolve' : 'accelerations',
        'twist' : 'yes',
        'overexisting' : 'yes',
        'spsolve' : 'no',
        'camera_angles_sigma' : .25,
        'camera_angular_velocity_sigma' : .1,
        'camera_angular_acceleration_sigma' : .01,
        'point_radius_sigma' : 50
    }

    try:
        jigsaw(**bundle_parameters)
    except ProcessError as e:
        print('Jigsaw Error')
        print("STDOUT:", e.stdout.decode('utf-8'))
        print("STDERR:", e.stderr.decode('utf-8'))

    df = pd.read_csv('residuals.csv', header=1)

    residuals = df.iloc[1:]['residual.1'].astype(float)

    residual_min = min(residuals)
    residual_max = max(residuals)

    df['residual'].iloc[1:].astype(float).describe()

    img1fh = GeoDataset(img1)
    img2fh = GeoDataset(img2)


    # need to clean up time stuff
    print("Image 1 =======")
    label = img1fh.metadata
    starttime1 = label['IsisCube']['Instrument']['StartTime']
    endtime1 = label['IsisCube']['Instrument']['StopTime']
    print(marstime.getMTfromTime(starttime1 + (endtime1-starttime1)/2)[0])
    print(label[4][1]['SolarLongitude'])
    print('LOCAL TIME', marstime.getLTfromTime(starttime1,0))

    print()
    print("Image 2 =======")
    label = img2fh.metadata
    starttime2 = label['IsisCube']['Instrument']['StartTime']
    endtime2 = label['IsisCube']['Instrument']['StopTime']

    print('start time: ', starttime2)
    earth_time2 = starttime2 + (endtime2-starttime2)/2
    print('EARTH_TIME: ', earth_time2)
    print(label[4][1]['SolarLongitude'])
    print('LOCAL TIME', marstime.getLTfromTime(starttime2,0))


    # Run spiceinit and footprintint - latter helps constrain the search
    for f in files:
        try:
            footprintinit(from_=f)
        except Exception as e:
            print('FP Error')
            print("STDOUT:", e.stdout.decode('utf-8'))
            print("STDERR:", e.stderr.decode('utf-8'))

    img1fh = GeoDataset(img1)
    img2fh = GeoDataset(img2)

    shape1 = wkt.loads(img1fh.footprint.GetGeometryRef(0).ExportToWkt())
    shape2 = wkt.loads(img2fh.footprint.GetGeometryRef(0).ExportToWkt())
    minlong, minlat, maxlong, maxlat = shape1.intersection(shape2).bounds

    img1proj = '{}.proj.cub'.format(img1.split('.')[0])
    img2proj = '{}.proj.cub'.format(img2.split('.')[0])

    print(img1proj)
    print(img2proj)

    cam2map_params1 = {
        'from_' : img1,
        'map' : 'equidistant.map',
        'to' : img1proj
    }

    cam2map_params2 = {
        'from_' : img2,
        'map' : img1proj,
        'to' : img2proj,
        'matchmap' : 'yes'
    }

    try:
        print('Running cam2map on {}'.format(img1))
        cam2map(**cam2map_params1)
        print('Running cam2map on {}'.format(img2))
        cam2map(**cam2map_params2)
    except ProcessError as e:
        print('cam2map Error')
        print("STDOUT:", e.stdout.decode('utf-8'))
        print("STDERR:", e.stderr.decode('utf-8'))

    args = ['deplaid=1','wnremove=1', 'autoradcorr=1', 'destreak=1']
    try:
        out1, err1 = run_davinci('thm_post_process.dv', img1proj, img1proj, args=args)
        out2, err2 = run_davinci('thm_post_process.dv', img2proj, img2proj, args=args)
    except Exception as e:
        print(e)

    try:
        out1, err1 = run_davinci('thm_tb.dv', img1proj, img1proj)
        out2, err2 = run_davinci('thm_tb.dv', img2proj, img2proj)
    except Exception as e:
        print(e)

    img1fh = GeoDataset(img1proj)
    img2fh = GeoDataset(img2proj)

    img1data = img1fh.read_array(9)
    img2data = img2fh.read_array(9)

    img1data = np.ma.MaskedArray(img1data, img1data == np.min(img1data))
    img2data = np.ma.MaskedArray(img2data, img2data == np.min(img2data))

    print(np.min(img1data), np.min(img2data))
    print(img2data.shape)
    print(img1data.shape)

    diff = np.ma.MaskedArray(img1data.data-img2data.data, img1data.mask | img2data.mask)

    hist(diff[~diff.mask], bins=200)
    diffavg = np.mean(diff)
    diffmin = np.min(diff)
    diffmax = np.max(diff)
    diffstddev = np.std(diff)
    diffmin, diffmax, diffavg, diffstddev

    img1vals = img1inter[~img1inter.mask]
    img2vals = img2inter[~img2inter.mask]

    img1min = np.min(img1vals)
    img1max = np.max(img1vals)
    img1min, img1max

    img2min = np.min(img2vals)
    img2max = np.max(img2vals)
    img2min, img2max

    img1avg = np.mean(img1inter)
    img2avg = np.mean(img2inter)
    img1avg, img2avg

    # write to csv
