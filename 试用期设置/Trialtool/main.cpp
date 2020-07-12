#include "trialtool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Trialtool w;
	w.show();
	return a.exec();
}
