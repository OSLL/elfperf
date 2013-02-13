#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
fclose\
:_ZNK7QPixmap6heightEv\
:_ZNK7QPixmap6isNullEv\
:_ZNK7QString3argERKS_iRK5QChar\
:_ZNK7QString3midEii\
:_ZNK7QWidget11mapToParentERK6QPoint\
:_ZNK7QWidget11paintEngineEv\
:_ZNK7QWidget14backgroundRoleEv\
:_ZNK7QWidget14heightForWidthEi\
:_ZNK7QWidget15minimumSizeHintEv\
:_ZNK7QWidget16inputMethodQueryEN2Qt16InputMethodQueryE\
:_ZNK7QWidget6layoutEv\
:_ZNK7QWidget6metricEN12QPaintDevice17PaintDeviceMetricE\
:_ZNK7QWidget7devTypeEv\
:_ZNK7QWidget7paletteEv\
:_ZNK7QWidget8sizeHintEv\
:_ZNK8QPalette5brushENS_10ColorGroupENS_9ColorRoleE\
:_ZNK9QGLWidget10metaObjectEv\
:_ZNK9QGLWidget11paintEngineEv\
:_ZTI13QWindowsStyle\
:_ZTI7QObject\
:_ZTI7QWidget\
:_ZTI9QGLWidget\
:_ZTISt9bad_alloc\
:_ZTV11ArthurFrame\
:_ZTV11ArthurStyle\
:_ZTV11HoverPoints\
:_ZTV8GLWidget\
:_ZTVN10__cxxabiv120__si_class_type_infoE\
:_ZThn16_NK7QWidget11paintEngineEv\
:_ZThn16_NK7QWidget6metricEN12QPaintDevice17PaintDeviceMetricE\
:_ZThn16_NK7QWidget7devTypeEv\
:_ZThn16_NK9QGLWidget11paintEngineEv\
:fwrite /tools/lib/ld-2.11.3.so ./affine'

