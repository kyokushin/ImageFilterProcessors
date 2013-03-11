#ifndef qt_lib_h
#define qt_lib_h

#include <string>
#include <qfiledialog.h>
#include <qcolordialog.h>

bool fileSelectDialog( std::string& fname ){
	QString qfname = QFileDialog::getOpenFileName(
				0, QString("open file"),
				QString(),
				QString("Images (*.png *.xpm *.jpg)"));
	fname = qfname.toUtf8().constData();
	if( fname == "" ) return false;
	return true;
}

bool colorPicker( cv::Scalar& color ){
	QColorDialog color_dialog;
	color_dialog.open();
	const QColor qcolor = color_dialog.selectedColor();
	color[0] = qcolor.blue();
	color[1] = qcolor.green();
	color[2] = qcolor.red();
}

#endif
