/**
	������У����
	���ʣ�exit ������������ϵ���飿
**/
#ifndef QVERIFICATION_H
#define QVERIFICATION_H

#include <QObject>

class QVerification : public QObject
{
	Q_OBJECT

public:
	QVerification(QObject *parent = NULL);
	~QVerification();

	//1������ʹ��
	//0������ʹ��
	//-1���Ҳ���xml�ļ�
	//-2��������XML
	//-3: ʱ�䱻�޸�
	//-4�����ý���
	//-5���ļ�����ʧ��
	//-6���ļ�����ʧ��

	int verification();

private:
	void writeRegedit(QString key, QString val);
	QString getRegedit(QString key);
	void removeRegedit(QString key);

	QString getNetTime();
	void firstRun(QString CDKey, int iDays, int iTimes, QString date);
};

#endif // QVERIFICATION_H
