#include "qverification.h"
#include "XmlTools.h"
#include <QMessageBox>
#include <QSettings>
#include <QTcpSocket>
#include <QDate>
#include <QCoreApplication>
#include "base64.h"

int mon[12] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
int cal(int y, int m, int d)
{
	return y * 365 + y / 4 - y / 100 + y / 400 +
		mon[m - 1] + d - 1 +
		(((y % 100 != 0 && y % 4 == 0) || y % 400 == 0) && m > 2);
}

QVerification::QVerification(QObject *parent)
	: QObject(parent)
{
	QCoreApplication::setOrganizationName("Zhangzhihao");
	QCoreApplication::setApplicationName("testTrailTool");
}

QVerification::~QVerification()
{
	qDebug()<<"jkl";
}

void QVerification::writeRegedit(QString key, QString val)
{
	QSettings Setting(QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	Setting.setValue(key, val);
}

QString QVerification::getRegedit(QString key)
{
	QSettings Setting(QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	QString val = Setting.value(key).toString();
	return val;
}

void QVerification::removeRegedit(QString key)
{
	QSettings Setting(QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	Setting.remove(key);
}

QString QVerification::getNetTime()
{
	QStringList urls;
	urls<<"time-b-g.nist.gov"<<"time-c-g.nist.gov"<<"time-d-g.nist.gov"
		<<"time-e-g.nist.gov"<<"time-a-wwv.nist.gov"<<"time-b-wwv.nist.gov"
		<<"time-c-wwv.nist.gov"<<"time-d-wwv.nist.gov"<<"time-e-wwv.nist.gov"
		<<"time-a-b.nist.gov"<<"time-b-b.nist.gov"<<"time-c-b.nist.gov"
		<<"time-d-b.nist.gov"<<"time-e-b.nist.gov"<<"time.nist.gov"
		<<"utcnist.colorado.edu"<<"utcnist2.colorado.edu";

	QString netTime;
	QTcpSocket *socket = new QTcpSocket();
	for (int i=0; i<urls.size(); i++)
	{
		socket->connectToHost(urls.at(i), 13);
		//QCoreApplication::processEvents();
		if (socket->waitForConnected())
		{
			if (socket->waitForReadyRead())
			{
				QString str(socket->readAll());
				netTime = str.trimmed();
				netTime = str.section(" ", 1, 2);
				qDebug()<<netTime;
				break;
			}
		}
		socket->close();
	}

	delete socket;
	return netTime;
}

int QVerification::verification()
{
	QString msg;
	Base64 base64;
	QString filePath = QCoreApplication::applicationDirPath().append("/").append("usage.xml");
	//Base64解密
	if (!base64.Decrypt(filePath))
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("读配置文件，解密失败!"));
		qDebug() << QString::fromLocal8Bit("读配置文件，解密失败!");
		return -5;
	}

	bool bRet = XmlTools::GetInstance()->LoadXmlFile(filePath.toLocal8Bit().constData());
	//找不到配置文件时失败
	if (!bRet)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("加载秘钥文件失败！"));

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("读配置文件，加密失败!");
			return -5;
		}
		return -1;
	}

	//获取xml信息
	bool bIsPermanent = QString(XmlTools::GetInstance()->GetValue("UsageConfig/IsPermanent")).trimmed().toInt();
	QString CDKey = QString(XmlTools::GetInstance()->GetValue("UsageConfig/CDKey")).trimmed();
	QString fisrtUsageDate = QString(XmlTools::GetInstance()->GetValue("UsageConfig/fisrtUsageDate")).trimmed();
	int iViableDays = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableDays")).trimmed().toInt();
	int iViableTimes = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableTimes")).trimmed().toInt();

	//永久使用
	if (bIsPermanent)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("你已获得永久使用权！"));
		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("读配置文件，加密失败!");
			return -5;
		}
		return 1;
	}

	//获取网络时间，失败则试用本地时间
	QString timeNet = getNetTime();
	QDateTime currentime = QDateTime::currentDateTime(); 
	if (timeNet.isNull())
	{
		timeNet = currentime.toString("yyyy-MM-dd hh:mm:ss"); 
	}
	else
	{
		timeNet = QString("20").append(timeNet);
	}

	//拆分日期
	int iYear, iMonth, iDay;
	QStringList temp = timeNet.split(" ").at(0).split("-");
	iYear = temp.at(0).toInt();
	iMonth = temp.at(1).toInt();
	iDay = temp.at(2).toInt();

	//次数减一
	iViableTimes--;
	QStringList dateList;
	dateList<<QString::number(iYear)<<QString::number(iMonth)<<QString::number(iDay);
	QString date = dateList.join(".");

	//先获取序列号
	QString cdKeyReg = getRegedit("cdKey");

	//如果不存在则是首次启动
	if (cdKeyReg.isNull())
	{
		//写入首次启动日期，更新xml日期及次数
		msg = QString::fromLocal8Bit("%1日后的24:00时或启动%2次后试用结束！").arg(iViableDays).arg(iViableTimes);
		QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);
		firstRun(CDKey, iViableDays, iViableTimes, date);

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("读配置文件，加密失败!");
			return -5;
		}
		return 0;
	}
	//否则如果是同一个秘钥文件，则判断首次启动日期
	else if(cdKeyReg == CDKey)
	{
		//如果日期一致，则判定为复制了初始XML
		if (fisrtUsageDate == "1995.09.02")
		{
			QMessageBox::critical(0, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("检测到可能的秘钥复制行为！"));
			if (!base64.Encrypt(filePath))
			{
				qDebug() << QStringLiteral("读配置文件，加密失败!");
				return -5;
			}
			return -2;
		}
		else//如果不一致
		{
			if (iViableDays > 0 && iViableTimes >= 0)
			{
				//计算时间间隔
				QStringList temp = fisrtUsageDate.split(".");
				int iyearBefor = temp.at(0).toInt();
				int imonthBefor = temp.at(1).toInt();
				int idayBefor = temp.at(2).toInt();

				int iDateBefore = cal(iyearBefor, imonthBefor, idayBefor);
				int iDateAfter = cal(iYear, iMonth, iDay);

				//检测时间是否被修改
				if (iDateBefore > iDateAfter)
				{
					QMessageBox::critical(0, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("检测到系统时间被改变！"));
					if (!base64.Encrypt(filePath))
					{
						qDebug() << QStringLiteral("读配置文件，加密失败!");
						return -5;
					}
					return -3;
				}

				//次数更新
				int iGap = iDateAfter - iDateBefore;
				iViableDays = iViableDays - iGap;
				if (iViableDays < 0)
				{
					msg = QString::fromLocal8Bit("软件试用已结束，请联系供应商注册！");
					QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);

					if (!base64.Encrypt(filePath))
					{
						qDebug() << QStringLiteral("读配置文件，加密失败!");
						return -5;
					}
					return -4;
				}

				XmlTools::GetInstance()->SetValue("UsageConfig/viableDays", QString::number(iViableDays).toStdString().c_str());
				XmlTools::GetInstance()->SetValue("UsageConfig/viableTimes", QString::number(iViableTimes).toStdString().c_str());

				msg = QString::fromLocal8Bit("%1日后的24:00时或启动%2次后试用结束！").arg(iViableDays).arg(iViableTimes);
				QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);

				if (!base64.Encrypt(filePath))
				{
					qDebug() << QStringLiteral("读配置文件，加密失败!");
					return -5;
				}
				return 0;
			}
			else
			{
				msg = QString::fromLocal8Bit("软件试用已结束，请联系供应商注册！");
				QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);

				if (!base64.Encrypt(filePath))
				{
					qDebug() << QStringLiteral("读配置文件，加密失败!");
					return -5;
				}
				return -4;
			}
		}
	}
	//否则如果不是同一个，则是新的秘钥文件要先清除注册表，当首次运行处理
	else
	{
		//写入首次启动日期，更新xml日期及次数
		msg = QString::fromLocal8Bit("%1日后的24:00时或启动%2次后试用结束！").arg(iViableDays).arg(iViableTimes);
		QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);
		firstRun(CDKey, iViableDays, iViableTimes, date);

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("读配置文件，加密失败!");
			return -5;
		}
		return 0;
	}
}

void QVerification::firstRun(QString CDKey, int iDays, int iTimes, QString date)
{
	//删除历史信息
	removeRegedit("cdKey");
	removeRegedit("firstRunDate");

	XmlTools::GetInstance()->SetValue("UsageConfig/fisrtUsageDate", date.toLocal8Bit().constData());
	XmlTools::GetInstance()->SetValue("UsageConfig/viableDays", QString::number(iDays).toStdString().c_str());
	XmlTools::GetInstance()->SetValue("UsageConfig/viableTimes", QString::number(iTimes).toStdString().c_str());

	//32位程序运行在WOW64模式下，setting被存储在 HKEY_LOCAL_MACHINE\Software\WOW6432Node上
	writeRegedit("firstRunDate", date);
	writeRegedit("cdKey", CDKey);
}
