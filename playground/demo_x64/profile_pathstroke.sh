#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
_Unwind_Resume\
:_Z20qt_qFindChild_helperPK7QObjectRK7QStringRK11QMetaObject\
:_Z21qRegisterResourceDataiPKhS0_S0_\
:_Z23qUnregisterResourceDataiPKhS0_S0_\
:_Z23qt_qFindChildren_helperPK7QObjectRK7QStringPK7QRegExpRK11QMetaObjectP5QListIPvE\
:_Z5qFreePv\
:_Z7qMemSetPvim\
:_Z9qBadAllocv\
:_ZN10QBoxLayout10addStretchEi\
:_ZN10QBoxLayout9addLayoutEP7QLayouti\
:_ZN10QBoxLayout9addWidgetEP7QWidgeti6QFlagsIN2Qt13AlignmentFlagEE\
:_ZN11QBasicTimer4stopEv\
:_ZN11QBasicTimer5startEiP7QObject\
:_ZN11QGridLayout7addItemEP11QLayoutItemiiii6QFlagsIN2Qt13AlignmentFlagEE\
:_ZN11QGridLayout9addLayoutEP7QLayoutii6QFlagsIN2Qt13AlignmentFlagEE\
:_ZN11QGridLayout9addWidgetEP7QWidgetii6QFlagsIN2Qt13AlignmentFlagEE\
:_ZN11QGridLayoutC1EP7QWidget\
:_ZN11QHBoxLayoutC1EP7QWidget\
:_ZN11QMetaObject8activateEP7QObjectPKS_iPPv\
:_ZN11QPushButton16staticMetaObjectE\
:_ZN11QPushButtonC1EP7QWidget\
:_ZN11QPushButtonC1ERK7QStringP7QWidget\
:_ZN11QTextCursorD1Ev\
:_ZN11QTextFormatD2Ev\
:_ZN11QTouchEvent10TouchPointC1ERKS0_\
:_ZN11QTouchEvent10TouchPointD1Ev\
:_ZN11QVBoxLayoutC1EP7QWidget\
:_ZN11QVectorData10reallocateEPS_iii\
:_ZN11QVectorData4freeEPS_i\
:_ZN11QVectorData4growEiiib\
:_ZN11QVectorData8allocateEii\
:_ZN12QApplication4execEv\
:_ZN12QApplication5syncXEv\
:_ZN12QApplicationC1ERiPPci\
:_ZN12QApplicationD1Ev\
:_ZN12QPainterPath10addEllipseERK6QRectF\
:_ZN12QPainterPath11setFillRuleEN2Qt8FillRuleE\
:_ZN12QPainterPath12closeSubpathEv\
:_ZN12QPainterPath5arcToERK6QRectFdd\
:_ZN12QPainterPath6lineToERK7QPointF\
:_ZN12QPainterPath6moveToERK7QPointF\
:_ZN12QPainterPath7addRectERK6QRectF\
:_ZN12QPainterPath7cubicToERK7QPointFS2_S2_\
:_ZN12QPainterPathC1Ev\
:_ZN12QPainterPathD1Ev\
:_ZN12QPixmapCache4findERK7QString\
:_ZN12QPixmapCache6insertERK7QStringRK7QPixmap\
:_ZN12QRadioButton16staticMetaObjectE\
:_ZN12QRadioButtonC1EP7QWidget\
:_ZN12QResizeEventC1ERK5QSizeS2_\
:_ZN12QStyleOptionD2Ev\
:_ZN12QStyleOptionaSERKS_\
:_ZN12QTextBrowser16staticMetaObjectE\
:_ZN12QTextBrowserC1EP7QWidget\
:_ZN13QTextDocument11setPageSizeERK6QSizeF\
:_ZN13QTextDocument7setHtmlERK7QString\
:_ZN13QTextDocumentC1EP7QObject\
:_ZN13QWindowsStyle10timerEventEP11QTimerEvent\
:_ZN13QWindowsStyle11eventFilterEP7QObjectP6QEvent\
:_ZN13QWindowsStyle11qt_metacallEN11QMetaObject4CallEiPPv\
:_ZN13QWindowsStyle11qt_metacastEPKc\
:_ZN13QWindowsStyle6polishEP12QApplication\
:_ZN13QWindowsStyle8unpolishEP12QApplication\
:_ZN13QWindowsStyleC2Ev\
:_ZN13QWindowsStyleD2Ev\
:_ZN15QAbstractButton10setCheckedEb\
:_ZN15QAbstractButton11setIconSizeERK5QSize\
:_ZN15QAbstractButton12setCheckableEb\
:_ZN15QAbstractButton7setIconERK5QIcon\
:_ZN15QAbstractButton7setTextERK7QString\
:_ZN15QAbstractSlider8setRangeEii\
:_ZN15QAbstractSlider8setValueEi\
:_ZN15QLinearGradientC1Edddd\
:_ZN16QCoreApplication9postEventEP7QObjectP6QEvent\
:_ZN19QPainterPathStroker11setCapStyleEN2Qt11PenCapStyleE\
:_ZN19QPainterPathStroker12setJoinStyleEN2Qt12PenJoinStyleE\
:_ZN19QPainterPathStroker14setDashPatternERK7QVectorIdE\
:_ZN19QPainterPathStroker8setWidthEd\
:_ZN19QPainterPathStrokerC1Ev\
:_ZN19QPainterPathStrokerD1Ev\
:_ZN19QStyleOptionComplexC2Eii\
:_ZN4QPenC1ERK6QBrushdN2Qt8PenStyleENS3_11PenCapStyleENS3_12PenJoinStyleE\
:_ZN4QPenD1Ev\
:_ZN5QCharC1Ec\
:_ZN5QFile4openE6QFlagsIN9QIODevice12OpenModeFlagEE\
:_ZN5QFileC1ERK7QString\
:_ZN5QFileD1Ev\
:_ZN5QIconC1ERK7QPixmap\
:_ZN5QIconD1Ev\
:_ZN6QBrushC1EN2Qt11GlobalColorENS0_10BrushStyleE\
:_ZN6QBrushC1ERK6QColorN2Qt10BrushStyleE\
:_ZN6QBrushC1ERK9QGradient\
:_ZN6QBrushC1ERKS_\
:_ZN6QBrushD1Ev\
:_ZN6QColor10invalidateEv\
:_ZN6QColor6setRgbEiiii\
:_ZN6QColorC1EN2Qt11GlobalColorE\
:_ZN6QColoraSERKS_\
:_ZN6QImageC1ERK5QSizeNS_6FormatE\
:_ZN6QImageC1ERK7QStringPKc\
:_ZN6QImageD1Ev\
:_ZN6QLabelC1ERK7QStringP7QWidget6QFlagsIN2Qt10WindowTypeEE\
:_ZN7QLayout10setSpacingEi\
:_ZN7QLayout9setMarginEi\
:_ZN7QMatrix5scaleEdd\
:_ZN7QMatrix5shearEdd\
:_ZN7QMatrix6rotateEd\
:_ZN7QMatrixC1Ev\
:_ZN7QObject10childEventEP11QChildEvent\
:_ZN7QObject10timerEventEP11QTimerEvent\
:_ZN7QObject11customEventEP6QEvent\
:_ZN7QObject11eventFilterEPS_P6QEvent\
:_ZN7QObject13connectNotifyEPKc\
:_ZN7QObject16disconnectNotifyEPKc\
:_ZN7QObject5eventEP6QEvent\
:_ZN7QObject7connectEPKS_PKcS1_S3_N2Qt14ConnectionTypeE\
:_ZN7QPixmap4fillERK6QColor\
:_ZN7QPixmap9fromImageERK6QImage6QFlagsIN2Qt19ImageConversionFlagEE\
:_ZN7QPixmapC1ERK7QStringPKc6QFlagsIN2Qt19ImageConversionFlagEE\
:_ZN7QPixmapC1ERKS_\
:_ZN7QPixmapC1Eii\
:_ZN7QPixmapC1Ev\
:_ZN7QPixmapD1Ev\
:_ZN7QPixmapaSERKS_\
:_ZN7QRegExp10setMinimalEb\
:_ZN7QRegExpC1ERK7QStringN2Qt15CaseSensitivityENS_13PatternSyntaxE\
:_ZN7QRegExpD1Ev\
:_ZN7QRegionC1ERK5QRectNS_10RegionTypeE\
:_ZN7QRegionD1Ev\
:_ZN7QSlider16staticMetaObjectE\
:_ZN7QSliderC1EN2Qt11OrientationEP7QWidget\
:_ZN7QString16fromAscii_helperEPKci\
:_ZN7QString17fromLatin1_helperEPKci\
:_ZN7QString4freeEPNS_4DataE\
:_ZN7QString6appendERKS_\
:_ZN7QString6insertEiPK5QChari\
:_ZN7QString7replaceE5QCharRKS_N2Qt15CaseSensitivityE\
:_ZN7QString7replaceERK7QRegExpRKS_\
:_ZN7QString7replaceERKS_S1_N2Qt15CaseSensitivityE\
:_ZN7QString9fromAsciiEPKci\
:_ZN7QStringaSERKS_\
:_ZN7QWidget10closeEventEP11QCloseEvent\
:_ZN7QWidget10enterEventEP6QEvent\
:_ZN7QWidget10fontChangeERK5QFont\
:_ZN7QWidget10leaveEventEP6QEvent\
:_ZN7QWidget10paintEventEP11QPaintEvent\
:_ZN7QWidget10setPaletteERK8QPalette\
:_ZN7QWidget10setVisibleEb\
:_ZN7QWidget10wheelEventEP11QWheelEvent\
:_ZN7QWidget11actionEventEP12QActionEvent\
:_ZN7QWidget11changeEventEP6QEvent\
:_ZN7QWidget11qt_metacallEN11QMetaObject4CallEiPPv\
:_ZN7QWidget11qt_metacastEPKc\
:_ZN7QWidget11resizeEventEP12QResizeEvent\
:_ZN7QWidget11setGeometryERK5QRect\
:_ZN7QWidget11styleChangeER6QStyle\
:_ZN7QWidget11tabletEventEP12QTabletEvent\
:_ZN7QWidget12focusInEventEP11QFocusEvent\
:_ZN7QWidget12setAttributeEN2Qt15WidgetAttributeEb\
:_ZN7QWidget13dragMoveEventEP14QDragMoveEvent\
:_ZN7QWidget13enabledChangeEb\
:_ZN7QWidget13focusOutEventEP11QFocusEvent\
:_ZN7QWidget13keyPressEventEP9QKeyEvent\
:_ZN7QWidget13paletteChangeERK8QPalette\
:_ZN7QWidget13setFixedWidthEi\
:_ZN7QWidget13setSizePolicyE11QSizePolicy\
:_ZN7QWidget14dragEnterEventEP15QDragEnterEvent\
:_ZN7QWidget14dragLeaveEventEP15QDragLeaveEvent\
:_ZN7QWidget14languageChangeEv\
:_ZN7QWidget14mouseMoveEventEP11QMouseEvent\
:_ZN7QWidget14setFixedHeightEi\
:_ZN7QWidget14setMinimumSizeEii\
:_ZN7QWidget14setWindowTitleERK7QString\
:_ZN7QWidget14showFullScreenEv\
:_ZN7QWidget15keyReleaseEventEP9QKeyEvent\
:_ZN7QWidget15mousePressEventEP11QMouseEvent\
:_ZN7QWidget16contextMenuEventEP17QContextMenuEvent\
:_ZN7QWidget16inputMethodEventEP17QInputMethodEvent\
:_ZN7QWidget17mouseReleaseEventEP11QMouseEvent\
:_ZN7QWidget18focusNextPrevChildEb\
:_ZN7QWidget21mouseDoubleClickEventEP11QMouseEvent\
:_ZN7QWidget21setAutoFillBackgroundEb\
:_ZN7QWidget22windowActivationChangeEb\
:_ZN7QWidget5eventEP6QEvent\
:_ZN7QWidget6resizeERK5QSize\
:_ZN7QWidget6updateEv\
:_ZN7QWidget8setStyleEP6QStyle\
:_ZN7QWidget8x11EventEP7_XEvent\
:_ZN7QWidget9dropEventEP10QDropEvent\
:_ZN7QWidget9hideEventEP10QHideEvent\
:_ZN7QWidget9moveEventEP10QMoveEvent\
:_ZN7QWidget9setParentEPS_6QFlagsIN2Qt10WindowTypeEE\
:_ZN7QWidget9showEventEP10QShowEvent\
:_ZN7QWidgetC2EPS_6QFlagsIN2Qt10WindowTypeEE\
:_ZN7QWidgetD2Ev\
:_ZN8QPainter10drawPixmapERK6QRectFRK7QPixmapS2_\
:_ZN8QPainter10drawPixmapERK7QPointFRK7QPixmap\
:_ZN8QPainter10strokePathERK12QPainterPathRK4QPen\
:_ZN8QPainter11drawEllipseERK6QRectF\
:_ZN8QPainter11setClipPathERK12QPainterPathN2Qt13ClipOperationE\
:_ZN8QPainter11setClipRectERK5QRectN2Qt13ClipOperationE\
:_ZN8QPainter11setClippingEb\
:_ZN8QPainter12drawPolylineEPK7QPointFi\
:_ZN8QPainter13setClipRegionERK7QRegionN2Qt13ClipOperationE\
:_ZN8QPainter13setRenderHintENS_10RenderHintEb\
:_ZN8QPainter14setBrushOriginERK7QPointF\
:_ZN8QPainter15drawTiledPixmapERK6QRectFRK7QPixmapRK7QPointF\
:_ZN8QPainter3endEv\
:_ZN8QPainter4saveEv\
:_ZN8QPainter5beginEP12QPaintDevice\
:_ZN8QPainter6setPenEN2Qt8PenStyleE\
:_ZN8QPainter6setPenERK4QPen\
:_ZN8QPainter6setPenERK6QColor\
:_ZN8QPainter7restoreEv\
:_ZN8QPainter8drawPathERK12QPainterPath\
:_ZN8QPainter8drawTextERK5QRectiRK7QStringPS0_\
:_ZN8QPainter8fillPathERK12QPainterPathRK6QBrush\
:_ZN8QPainter8fillRectERK5QRectRK6QBrush\
:_ZN8QPainter8fillRectERK5QRectRK6QColor\
:_ZN8QPainter8fillRectERK6QRectFRK6QColor\
:_ZN8QPainter8setBrushEN2Qt10BrushStyleE\
:_ZN8QPainter8setBrushERK6QBrush\
:_ZN8QPainter9drawImageERK6QRectFRK6QImageS2_6QFlagsIN2Qt19ImageConversionFlagEE\
:_ZN8QPainter9drawRectsEPK5QRecti\
:_ZN8QPainter9translateERK7QPointF\
:_ZN8QPainterC1EP12QPaintDevice\
:_ZN8QPainterC1Ev\
:_ZN8QPainterD1Ev\
:_ZN8QPalette8setBrushENS_10ColorGroupENS_9ColorRoleERK6QBrush\
:_ZN8QPaletteC1ERKS_\
:_ZN8QPaletteC1Ev\
:_ZN8QPaletteD1Ev\
:_ZN8QPaletteaSERKS_\
:_ZN9QGLFormat10setStencilEb\
:_ZN9QGLFormat13defaultFormatEv\
:_ZN9QGLFormat16setDefaultFormatERKS_\
:_ZN9QGLFormat16setSampleBuffersEb\
:_ZN9QGLFormat18setAlphaBufferSizeEi\
:_ZN9QGLFormat8setAlphaEb\
:_ZN9QGLFormat9hasOpenGLEv\
:_ZN9QGLFormatC1E6QFlagsIN3QGL12FormatOptionEEi\
:_ZN9QGLFormatD1Ev\
:_ZN9QGLWidget11qt_metacallEN11QMetaObject4CallEiPPv\
:_ZN9QGLWidget11qt_metacastEPKc\
:_ZN9QGLWidget11resizeEventEP12QResizeEvent\
:_ZN9QGLWidget11swapBuffersEv\
:_ZN9QGLWidget12initializeGLEv\
:_ZN9QGLWidget14paintOverlayGLEv\
:_ZN9QGLWidget15resizeOverlayGLEii\
:_ZN9QGLWidget15updateOverlayGLEv\
:_ZN9QGLWidget17setAutoBufferSwapEb\
:_ZN9QGLWidget19initializeOverlayGLEv\
:_ZN9QGLWidget5eventEP6QEvent\
:_ZN9QGLWidget6glDrawEv\
:_ZN9QGLWidget6glInitEv\
:_ZN9QGLWidget7paintGLEv\
:_ZN9QGLWidget8resizeGLEii\
:_ZN9QGLWidget8updateGLEv\
:_ZN9QGLWidgetC2ERK9QGLFormatP7QWidgetPKS_6QFlagsIN2Qt10WindowTypeEE\
:_ZN9QGLWidgetD2Ev\
:_ZN9QGradient10setColorAtEdRK6QColor\
:_ZN9QGroupBox16staticMetaObjectE\
:_ZN9QGroupBox8setTitleERK7QString\
:_ZN9QGroupBoxC1EP7QWidget\
:_ZN9QHashData11free_helperEPFvPNS_4NodeEE\
:_ZN9QHashData12allocateNodeEi\
:_ZN9QHashData14detach_helper2EPFvPNS_4NodeEPvEPFvS1_Eii\
:_ZN9QHashData6rehashEi\
:_ZN9QHashData8freeNodeEPv\
:_ZN9QHashData8nextNodeEPNS_4NodeE\
:_ZN9QIODevice7readAllEv\
:_ZN9QListData6appendEv\
:_ZN9QListData7detach3Ev\
:_ZN9QTextEdit15setLineWrapModeENS_12LineWrapModeE\
:_ZN9QTextEdit7setHtmlERK7QString\
:_ZNK11QMetaObject2trEPKcS1_\
:_ZNK11QMetaObject4castEP7QObject\
:_ZNK11QTouchEvent10TouchPoint2idEv\
:_ZNK11QTouchEvent10TouchPoint3posEv\
:_ZNK11QTouchEvent10TouchPoint5stateEv\
:_ZNK12QCommonStyle14subControlRectEN6QStyle14ComplexControlEPK19QStyleOptionComplexNS0_10SubControlEPK7QWidget\
:_ZNK12QCommonStyle19generatedIconPixmapEN5QIcon4ModeERK7QPixmapPK12QStyleOption\
:_ZNK12QCommonStyle21hitTestComplexControlEN6QStyle14ComplexControlEPK19QStyleOptionComplexRK6QPointPK7QWidget\
:_ZNK12QFontMetrics5widthERK7QStringi\
:_ZNK13QTextDocument14documentLayoutEv\
:_ZNK13QTextDocument8pageSizeEv\
:_ZNK13QWindowsStyle10metaObjectEv\
:_ZNK13QWindowsStyle11drawControlEN6QStyle14ControlElementEPK12QStyleOptionP8QPainterPK7QWidget\
:_ZNK13QWindowsStyle11pixelMetricEN6QStyle11PixelMetricEPK12QStyleOptionPK7QWidget\
:_ZNK13QWindowsStyle13drawPrimitiveEN6QStyle16PrimitiveElementEPK12QStyleOptionP8QPainterPK7QWidget\
:_ZNK13QWindowsStyle14standardPixmapEN6QStyle14StandardPixmapEPK12QStyleOptionPK7QWidget\
:_ZNK13QWindowsStyle14subElementRectEN6QStyle10SubElementEPK12QStyleOptionPK7QWidget\
:_ZNK13QWindowsStyle16sizeFromContentsEN6QStyle12ContentsTypeEPK12QStyleOptionRK5QSizePK7QWidget\
:_ZNK13QWindowsStyle18drawComplexControlEN6QStyle14ComplexControlEPK19QStyleOptionComplexP8QPainterPK7QWidget\
:_ZNK13QWindowsStyle9styleHintEN6QStyle9StyleHintEPK12QStyleOptionPK7QWidgetP16QStyleHintReturn\
:_ZNK19QPainterPathStroker12createStrokeERK12QPainterPath\
:_ZNK6QImage4sizeEv\
:_ZNK6QLineF6lengthEv\
:_ZNK6QPoint15manhattanLengthEv\
:_ZNK6QStyle12drawItemTextEP8QPainterRK5QRectiRK8QPalettebRK7QStringNS5_9ColorRoleE\
:_ZNK6QStyle12itemTextRectERK12QFontMetricsRK5QRectibRK7QString\
:_ZNK6QStyle14drawItemPixmapEP8QPainterRK5QRectiRK7QPixmap\
:_ZNK6QStyle14itemPixmapRectERK5QRectiRK7QPixmap\
:_ZNK6QStyle15standardPaletteEv\
:_ZNK7QMatrix3mapERK7QPointF\
:_ZNK7QMatrixmlERKS_\
:_ZNK7QObject10objectNameEv\
:_ZNK7QPixmap10pixmapDataEv\
:_ZNK7QPixmap16x11PictureHandleEv\
:_ZNK7QPixmap4sizeEv\
:_ZNK7QPixmap5widthEv\
:_ZNK7QPixmap6heightEv\
:_ZNK7QPixmap6isNullEv\
:_ZNK7QString3argERKS_iRK5QChar\
:_ZNK7QString3midEii\
:_ZNK7QStringeqERK13QLatin1String\
:_ZNK7QStringeqERKS_\
:_ZNK7QWidget11mapToParentERK6QPoint\
:_ZNK7QWidget11minimumSizeEv\
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
:_ZTI9QGLWidget\
:_ZThn16_NK7QWidget11paintEngineEv\
:_ZThn16_NK7QWidget6metricEN12QPaintDevice17PaintDeviceMetricE\
:_ZThn16_NK7QWidget7devTypeEv\
:_ZThn16_NK9QGLWidget11paintEngineEv\
:_ZdlPv\
:_Znwm\
:__cxa_atexit\
:__cxa_begin_catch\
:__cxa_end_catch\
:__cxa_rethrow\
:__gxx_personality_v0\
:__libc_start_main\
:memcpy   /tools/lib/ld-2.11.3.so ./pathstroke'

