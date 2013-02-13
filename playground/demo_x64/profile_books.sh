#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
_ZN16QCoreApplication9translateEPKcS1_S1_NS_8EncodingE\
:_ZN17QAbstractItemView15setCurrentIndexERK11QModelIndex\
:_ZN17QAbstractItemView15setItemDelegateEP21QAbstractItemDelegate\
:_ZN17QAbstractItemView16setSelectionModeENS_13SelectionModeE\
:_ZN17QAbstractItemView20setSelectionBehaviorENS_17SelectionBehaviorE\
:_ZN17QDataWidgetMapper10addMappingEP7QWidgeti\
:_ZN17QDataWidgetMapper15setItemDelegateEP21QAbstractItemDelegate\
:_ZN17QDataWidgetMapper8setModelEP18QAbstractItemModel\
:_ZN17QDataWidgetMapperC1EP7QObject\
:_ZN22QStyleOptionViewItemV3C1ERK20QStyleOptionViewItem\
:_ZN24QSqlRelationalTableModelC1EP7QObject12QSqlDatabase\
:_ZN4QPenC1ERKS_\
:_ZN4QPenD1Ev\
:_ZN5QDateC1Eiii\
:_ZN5QFontD1Ev\
:_ZN6QLabel7setTextERK7QString\
:_ZN6QLabelC1EP7QWidget6QFlagsIN2Qt10WindowTypeEE\
:_ZN7QLayout18setContentsMarginsEiiii\
:_ZN7QObject10childEventEP11QChildEvent\
:_ZN7QObject10timerEventEP11QTimerEvent\
:_ZN7QObject11customEventEP6QEvent\
:_ZN7QObject11eventFilterEPS_P6QEvent\
:_ZN7QObject13connectNotifyEPKc\
:_ZN7QObject13setObjectNameERK7QString\
:_ZN7QObject16disconnectNotifyEPKc\
:_ZN7QObject18installEventFilterEPS_\
:_ZN7QObject5eventEP6QEvent\
:_ZN7QObject7connectEPKS_PKcS1_S3_N2Qt14ConnectionTypeE\
:_ZN7QPixmapC1ERK7QStringPKc6QFlagsIN2Qt19ImageConversionFlagEE\
:_ZN7QPixmapD1Ev\
:_ZN7QString16fromAscii_helperEPKci\
:_ZN7QString17fromLatin1_helperEPKci\
:_ZN7QString4freeEPNS_4DataE\
:_ZN7QString6appendERKS_\
:_ZN7QString8fromUtf8EPKci\
:_ZN7QString9fromAsciiEPKci\
:_ZN7QWidget10closeEventEP11QCloseEvent\
:_ZN7QWidget10enterEventEP6QEvent\
:_ZN7QWidget10fontChangeERK5QFont\
:_ZN7QWidget10leaveEventEP6QEvent\
:_ZN7QWidget10paintEventEP11QPaintEvent\
:_ZN7QWidget10setEnabledEb\
:_ZN7QWidget10setVisibleEb\
:_ZN7QWidget10wheelEventEP11QWheelEvent\
:_ZN7QWidget11actionEventEP12QActionEvent\
:_ZN7QWidget11changeEventEP6QEvent\
:_ZN7QWidget11resizeEventEP12QResizeEvent\
:_ZN7QWidget11setTabOrderEPS_S0_\
:_ZN7QWidget11styleChangeER6QStyle\
:_ZN7QWidget11tabletEventEP12QTabletEvent\
:_ZN7QWidget12focusInEventEP11QFocusEvent\
:_ZN7QWidget13dragMoveEventEP14QDragMoveEvent\
:_ZN7QWidget13enabledChangeEb\
:_ZN7QWidget13focusOutEventEP11QFocusEvent\
:_ZN7QWidget13keyPressEventEP9QKeyEvent\
:_ZN7QWidget13paletteChangeERK8QPalette\
:_ZN7QWidget14dragEnterEventEP15QDragEnterEvent\
:_ZN7QWidget14dragLeaveEventEP15QDragLeaveEvent\
:_ZN7QWidget14languageChangeEv\
:_ZN7QWidget14mouseMoveEventEP11QMouseEvent\
:_ZN7QWidget14setWindowTitleERK7QString\
:_ZN7QWidget15keyReleaseEventEP9QKeyEvent\
:_ZN7QWidget15mousePressEventEP11QMouseEvent\
:_ZN7QWidget16inputMethodEventEP17QInputMethodEvent\
:_ZN7QWidget17mouseReleaseEventEP11QMouseEvent\
:_ZN7QWidget18focusNextPrevChildEb\
:_ZN7QWidget21mouseDoubleClickEventEP11QMouseEvent\
:_ZN7QWidget22windowActivationChangeEb\
:_ZN7QWidget6resizeERK5QSize\
:_ZN7QWidget8x11EventEP7_XEvent\
:_ZN7QWidget9dropEventEP10QDropEvent\
:_ZN7QWidget9hideEventEP10QHideEvent\
:_ZN7QWidget9moveEventEP10QMoveEvent\
:_ZN7QWidget9showEventEP10QShowEvent\
:_ZN7QWidgetC1EPS_6QFlagsIN2Qt10WindowTypeEE\
:_ZN8QPainter10drawPixmapERK7QPointFRK7QPixmap\
:_ZN8QPainter6setPenERK4QPen\
:_ZN8QPainter6setPenERK6QColor\
:_ZN8QPainter8fillRectERK5QRectRK6QColor\
:_ZN8QPainter9drawLinesEPK5QLinei\
:_ZN8QSpinBox10setMaximumEi\
:_ZN8QSpinBox10setMinimumEi\
:_ZN8QSpinBox9setPrefixERK7QString\
:_ZN8QSpinBoxC1EP7QWidget\
:_ZN8QVariantC1ERK5QDate\
:_ZN8QVariantC1ERK7QString\
:_ZN8QVariantC1Ei\
:_ZN8QVariantD1Ev\
:_ZN9QComboBox14setModelColumnEi\
:_ZN9QComboBox15setCurrentIndexEi\
:_ZN9QComboBox8setModelEP18QAbstractItemModel\
:_ZN9QComboBoxC1EP7QWidget\
:_ZN9QGroupBox8setTitleERK7QString\
:_ZN9QGroupBoxC1EP7QWidget\
:_ZN9QLineEditC1EP7QWidget\
:_ZN9QSqlErrorC1ERK7QStringS2_NS_9ErrorTypeEi\
:_ZN9QSqlErrorD1Ev\
:_ZN9QSqlQuery12addBindValueERK8QVariant6QFlagsIN4QSql13ParamTypeFlagEE\
:_ZN9QSqlQuery4execERK7QString\
:_ZN9QSqlQuery4execEv\
:_ZN9QSqlQuery7prepareERK7QString\
:_ZN9QSqlQueryC1ERK7QString12QSqlDatabase\
:_ZN9QSqlQueryD1Ev\
:_ZN9QtPrivate20QStringList_containsEPK11QStringListRK7QStringN2Qt15CaseSensitivityE\
:_ZNK11QMetaObject2trEPKcS1_\
:_ZNK11QMetaObject4castEP7QObject\
:_ZNK12QSqlDatabase6tablesEN4QSql9TableTypeE\
:_ZNK12QSqlDatabase9lastErrorEv\
:_ZNK13QItemDelegate10metaObjectEv\
:_ZNK13QItemDelegate11drawDisplayEP8QPainterRK20QStyleOptionViewItemRK5QRectRK7QString\
:_ZNK13QItemDelegate12createEditorEP7QWidgetRK20QStyleOptionViewItemRK11QModelIndex\
:_ZNK13QItemDelegate12setModelDataEP7QWidgetP18QAbstractItemModelRK11QModelIndex\
:_ZNK13QItemDelegate13setEditorDataEP7QWidgetRK11QModelIndex\
:_ZNK13QItemDelegate14drawDecorationEP8QPainterRK20QStyleOptionViewItemRK5QRectRK7QPixmap\
:_ZNK13QItemDelegate20updateEditorGeometryEP7QWidgetRK20QStyleOptionViewItemRK11QModelIndex\
:_ZNK13QItemDelegate5paintEP8QPainterRK20QStyleOptionViewItemRK11QModelIndex\
:_ZNK13QItemDelegate8sizeHintERK20QStyleOptionViewItemRK11QModelIndex\
:_ZNK13QItemDelegate9drawCheckEP8QPainterRK20QStyleOptionViewItemRK5QRectN2Qt10CheckStateE\
:_ZNK13QItemDelegate9drawFocusEP8QPainterRK20QStyleOptionViewItemRK5QRect\
:_ZNK14QSqlQueryModel9lastErrorEv\
:_ZNK14QSqlTableModel10fieldIndexERK7QString\
:_ZNK17QAbstractItemView14selectionModelEv\
:_ZNK24QSqlRelationalTableModel8relationEi\
:_ZNK7QObject10objectNameEv\
:_ZNK7QPixmap5widthEv\
:_ZNK7QPixmap6heightEv\
:_ZNK7QWidget11paintEngineEv\
:_ZNK7QWidget14heightForWidthEi\
:_ZNK7QWidget15minimumSizeHintEv\
:_ZNK7QWidget16inputMethodQueryEN2Qt16InputMethodQueryE\
:_ZNK7QWidget6metricEN12QPaintDevice17PaintDeviceMetricE\
:_ZNK7QWidget7devTypeEv\
:_ZNK7QWidget8sizeHintEv\
:_ZNK8QPainter3penEv\
:_ZNK8QPalette5brushENS_10ColorGroupENS_9ColorRoleE\
:_ZNK8QVariant5toIntEPb\
:_ZNK8QVariant8toStringEv\
:_ZNK9QComboBox12currentIndexEv\
:_ZNK9QComboBox8findDataERK8QVarianti6QFlagsIN2Qt9MatchFlagEE\
:_ZNK9QSqlError4textEv\
:_ZNK9QSqlError4typeEv\
:_ZNK9QSqlQuery12lastInsertIdEv\
:_ZNK9QSqlQuery9lastErrorEv\
:_ZThn16_NK7QWidget11paintEngineEv\
:_ZThn16_NK7QWidget6metricEN12QPaintDevice17PaintDeviceMetricE\
:_ZThn16_NK7QWidget7devTypeEv  /tools/lib/ld-2.11.3.so ./books'

