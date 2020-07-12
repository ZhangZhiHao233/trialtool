/**
	试用期校验类
	疑问：exit 与析构函数关系调查？
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

	//1：永久使用
	//0：可以使用
	//-1：找不到xml文件
	//-2：复制了XML
	//-3: 时间被修改
	//-4：试用结束
	//-5：文件解密失败
	//-6：文件加密失败

	int verification();

private:
	void writeRegedit(QString key, QString val);
	QString getRegedit(QString key);
	void removeRegedit(QString key);

	QString getNetTime();
	void firstRun(QString CDKey, int iDays, int iTimes, QString date);
};

#endif // QVERIFICATION_H
