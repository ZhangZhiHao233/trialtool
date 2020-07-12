#include "testtrialtool.h"
#include <QtWidgets/QApplication>
#include <Windows.h>
#include <QMessageBox>
#include <QDebug>

HANDLE g_mutex = NULL;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	g_mutex = CreateMutex(NULL,TRUE, L"NoRepeat");

	if(NULL == g_mutex)
	{
		qDebug()<<"CreateMutex failed!";
	}

	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��������Ѵ��ڣ�"));
		CloseHandle(g_mutex);
		return FALSE;
	}

	testTrialTool w;
	w.show();
	return a.exec();
}
