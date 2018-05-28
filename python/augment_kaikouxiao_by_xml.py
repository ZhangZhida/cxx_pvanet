#!/usr/bin/env python
# -*- coding:utf-8 -*-
from __future__ import print_function, division
import sys
sys.path.append('../')

import numpy as np
from scipy import misc
from imgaug import augmenters as iaa
import imgaug as ia

from scipy import ndimage, misc
from skimage import data

from xml.etree import ElementTree
from xml.etree.ElementTree import Element, SubElement
from lxml import etree
import codecs

import os

# copy xml to generate pascal voc xlm
# copy.deepcopy
import copy

XML_EXT = '.xml'

class PascalVocReader:

    def __init__(self, filepath):
        self.polygons = []
        self.filepath = filepath
        self.verified = False
        self.parseXML()

    def prettify(self, elem):
        """
            Return a pretty-printed XML string for the Element.
        """
        rough_string = ElementTree.tostring(elem, 'utf8')
        root = etree.fromstring(rough_string)
        return etree.tostring(root, pretty_print=True)

    def gettype(self):
        type='None'
        assert self.filepath.endswith('.xml'), "Unsupport file format"
        parser = etree.XMLParser(encoding='utf-8')
        xmltree = ElementTree.parse(self.filepath, parser=parser).getroot()
        for object_iter in xmltree.findall('object'):
            typetemp=object_iter.find('name').text
            '''
	    return type with priority
            '''
            if typetemp=='ZJ_zhe':
                type=typetemp
                return type
            else:
                type = typetemp
        return type



    # parse XML to get polygons
    def parseXML(self):
        assert self.filepath.endswith('.xml'), "Unsupport file format"
        parser = etree.XMLParser(encoding='utf-8')
        xmltree = ElementTree.parse(self.filepath, parser=parser).getroot()
        self.root = xmltree
        filename = xmltree.find('filename').text
        try:
            verified = xmltree.attrib['verified']
            if verified == 'yes':
                self.verified = True
        except KeyError:
            self.verified = False

        for object_iter in xmltree.findall('object'):
            #polygon = object_iter.find("polygon")
            bndbox = object_iter.find('bndbox')
            self.addPolygon(bndbox)
        return True

    # get polygons
    def getPolygons(self):
        return self.polygons
    # form xml get polygon and append it in polygons
    def addPolygon(self, polygon):
        #points = []
        #for point in polygon.findall('point'):
           # points.append(eval(point.text)) #eval
        xmin = eval(polygon.find('xmin').text)
        ymin = eval(polygon.find('ymin').text)
        xmax = eval(polygon.find('xmax').text)
        ymax = eval(polygon.find('ymax').text)
        points = [(xmin,ymin),(xmin,ymax),(xmax,ymin),(xmax,ymax)]
        self.polygons.append(points)

    # edit the polygons(data augmentation)
    def editPolygons(self,polygons):
        # xml_tree = copy.deepcopy(self.root)
        self.polygons = polygons
        # num = 0
        # for object_iter in xml_tree.findall('object'):
        #     # remove polygon
        #     bndbox = object_iter.find('bndbox')
        #     object_iter.remove(bndbox)
        #     polygon_iter = SubElement(object_iter, 'polygon')
        #     id = 0
        #     for point in polygons[num]:
        #         point_iter = SubElement(polygon_iter, 'point')
        #         point_iter.text = str(polygons[num][id])
        #         id = id + 1
        #     assert len(polygon)==id, 'numbers of points are not matched'
        #     num = num + 1 #the last element's id is len(polygon)-1
        #     return xml_tree

    # edit the filename(data augmentation)
    def editFilename(self, filename):
        fl_name = self.root.find('filename')
        fl_name.text = str(fl_name[0:-4])+'_'+filename+'.jpg'
#        fl_name.text = filename
    # save xml(data augmentation)
    def saveXML(self, xml_tree, targetFile=None):
        out_file = None
        if targetFile is None:
            out_file = codecs.open(
                self.filename + XML_EXT, 'w', encoding='utf-8')
        else:
            out_file = codecs.open(targetFile, 'w', encoding='utf-8')
        prettifyResult = self.prettify(xml_tree)
        out_file.write(prettifyResult.decode('utf8'))
        out_file.close()

    # convert polygon to bndbox(pascal voc xml)
    def convertPolygon2BndBox(self,polygon):
        xmin = float('inf')
        ymin = float('inf')
        xmax = float('-inf')
        ymax = float('-inf')
        for p in polygon:
            x = p[0]
            y = p[1]
            xmin = min(x, xmin)
            ymin = min(y, ymin)
            xmax = max(x, xmax)
            ymax = max(y, ymax)
        # Martin Kersner, 2015/11/12
        # 0-valued coordinates of BB caused an error while
        # training faster-rcnn object detector.
        if xmin < 1:
            xmin = 1
        if ymin < 1:
            ymin = 1
        return (int(xmin), int(ymin), int(xmax), int(ymax))

    def savePascalVocXML(self,targetFile=None):
        pascal_voc_tree = copy.deepcopy(self.root)
        num = 0
        for object_iter in pascal_voc_tree.findall('object'):
            bnd_box = self.convertPolygon2BndBox(self.polygons[num])
            #bndbox = SubElement(object_iter, 'bndbox')
            bndbox = object_iter.find("bndbox")
            xmin = bndbox.find('xmin')
            xmin.text = str(bnd_box[0])
            ymin = bndbox.find('ymin')
            ymin.text = str(bnd_box[1])
            xmax = bndbox.find('xmax')
            xmax.text = str(bnd_box[2])
            ymax = bndbox.find('ymax')
            ymax.text = str(bnd_box[3])
            num = num + 1
        out_file = None
        if targetFile is None:
            out_file = codecs.open(
                self.filename + XML_EXT, 'w', encoding='utf-8')
        else:
            out_file = codecs.open(targetFile, 'w', encoding='utf-8')
        prettifyResult = self.prettify(pascal_voc_tree)
        out_file.write(prettifyResult.decode('utf8'))
        out_file.close()


if __name__ == '__main__':
    #image = misc.imread('test02.jpg')
    #image = misc.imresize(ndimage.imread("quokka.jpg")[0:643, 0:643], (128, 128))
    '''
	modify output xml,pic dir
    '''
    save_voc_dir = "/home/cuizhou/projects/KaiKouXiao/2ndVOCdevkit2007_2ndround"

    '''
	modify input xml,pic dir
    '''
    xml_dir = os.path.join('/home/cuizhou/projects/KaiKouXiao/2ndVOCdevkit2007_2ndround/xml')
    img_dir = os.path.join('/home/cuizhou/projects/KaiKouXiao/2ndVOCdevkit2007_2ndround/img')

    xmls = os.listdir(xml_dir)
    imgs = os.listdir(img_dir)
    for xml_ in xmls:
        augnum=1
        img = xml_[0:-4]+'.jpg'
        image = ndimage.imread(os.path.join(img_dir,img))
        if image.shape[0]<1:
            continue
        name=os.path.splitext(img)[0]
        print (name)


        # get polygons from xml
        reader = PascalVocReader(os.path.join(xml_dir,xml_))
        my_type = reader.gettype()
        augnum=4
        if my_type=='ZJ_zhe':
            augnum=2

        if augnum!=1:
            # reader.savePascalVocXML('pascal/' + 'test01.xml')
            polygons = reader.getPolygons()
            keypoints = []
            for polygon in polygons:
                keypoints.extend([ia.Keypoint(point[0], point[1]) for point in polygon])
            keypoints = [ia.KeypointsOnImage(keypoints, shape=image.shape)]
            # draw on image
            image_keypoints = keypoints[0].draw_on_image(image, size=5)
            seq = iaa.Sequential([
                #iaa.Crop(px=(0, 16)),  # crop images from each side by 0 to 16px (randomly chosen)
                #iaa.Fliplr(0.5),  # horizontally flip 50% of the images
                #iaa.Flipud(0.5),
                iaa.Affine(rotate=(-6,6)),
                #iaa.Affine(#scale={"x": (0.5, 2.0), "y": (0.5, 2.0)},
                           # scale images to 80-120% of their size, individually per axis
                           #translate_px={"x": (-16, 16), "y": (-16, 16)},
                iaa.Add((-10, 10)),
                iaa.Affine(scale=(0.7,1.0)),
                #     shear=(-30, 30)),
                iaa.Multiply((0.5, 1.5)),  # change brightness of images (50-150% of original value)
                #iaa.ContrastNormalization((0.8, 1.5))
                #iaa.ElasticTransformation(alpha=(0.5, 3.5), sigma=0.25),
                #iaa.GaussianBlur((0, 1.5))
            ])
            for i in range(augnum):
                # to_deterministic to make image and keypoints to a batch, so
                seq_det = seq.to_deterministic()
                image_aug = seq_det.augment_image(image)
                keypoints_aug = seq_det.augment_keypoints(keypoints)[0]
                # convert from keypoints to array, or else can't make it to list in below loop
                new_points = keypoints_aug.get_coords_array()
                new_polygons = []
                bndBoxs = []
                num = 0
                for polygon in polygons:
                    # __str__ is class Keypoint's function
                    # eval -- change str to tuple,list,dist and so on
                    new_polygon = [(new_points[num + ind][0], new_points[num + ind][1]) for ind in range(len(polygon))]
                    num = num + len(polygon)
                    new_polygons.append(new_polygon)
                reader.editFilename(str(i))
                reader.editPolygons(new_polygons)
                # reader.saveXML(xml_tree,'my/'+str(i)+'.xml')
                reader.savePascalVocXML(save_voc_dir+'/Annotations/' + xml_[0:-4] + '_'+str(i) + '.xml')
                #aug_keypoints = keypoints_aug.draw_on_image(image_aug, size=5)
                #misc.imsave('img/' + xml_[0:-4]+ str(i) + '.jpg', aug_keypoints)
                misc.imsave(save_voc_dir+'/JPEGImages/' + xml_[0:-4] + '_'+str(i) + '.jpg', image_aug)

        else:
            misc.imsave(save_voc_dir+'/JPEGImages/'+img ,image)
            reader.savePascalVocXML(save_voc_dir+'/Annotations/' + xml_)


