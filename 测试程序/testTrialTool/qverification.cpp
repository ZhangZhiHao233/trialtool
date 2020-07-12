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
	//Base64����
	if (!base64.Decrypt(filePath))
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�������ļ�������ʧ��!"));
		qDebug() << QString::fromLocal8Bit("�������ļ�������ʧ��!");
		return -5;
	}

	bool bRet = XmlTools::GetInstance()->LoadXmlFile(filePath.toLocal8Bit().constData());
	//�Ҳ��������ļ�ʱʧ��
	if (!bRet)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("������Կ�ļ�ʧ�ܣ�"));

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("�������ļ�������ʧ��!");
			return -5;
		}
		return -1;
	}

	//��ȡxml��Ϣ
	bool bIsPermanent = QString(XmlTools::GetInstance()->GetValue("UsageConfig/IsPermanent")).trimmed().toInt();
	QString CDKey = QString(XmlTools::GetInstance()->GetValue("UsageConfig/CDKey")).trimmed();
	QString fisrtUsageDate = QString(XmlTools::GetInstance()->GetValue("UsageConfig/fisrtUsageDate")).trimmed();
	int iViableDays = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableDays")).trimmed().toInt();
	int iViableTimes = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableTimes")).trimmed().toInt();

	//����ʹ��
	if (bIsPermanent)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("���ѻ������ʹ��Ȩ��"));
		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("�������ļ�������ʧ��!");
			return -5;
		}
		return 1;
	}

	//��ȡ����ʱ�䣬ʧ�������ñ���ʱ��
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

	//�������
	int iYear, iMonth, iDay;
	QStringList temp = timeNet.split(" ").at(0).split("-");
	iYear = temp.at(0).toInt();
	iMonth = temp.at(1).toInt();
	iDay = temp.at(2).toInt();

	//������һ
	iViableTimes--;
	QStringList dateList;
	dateList<<QString::number(iYear)<<QString::number(iMonth)<<QString::number(iDay);
	QString date = dateList.join(".");

	//�Ȼ�ȡ���к�
	QString cdKeyReg = getRegedit("cdKey");

	//��������������״�����
	if (cdKeyReg.isNull())
	{
		//д���״��������ڣ�����xml���ڼ�����
		msg = QString::fromLocal8Bit("%1�պ��24:00ʱ������%2�κ����ý�����").arg(iViableDays).arg(iViableTimes);
		QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), msg);
		firstRun(CDKey, iViableDays, iViableTimes, date);

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("�������ļ�������ʧ��!");
			return -5;
		}
		return 0;
	}
	//���������ͬһ����Կ�ļ������ж��״���������
	else if(cdKeyReg == CDKey)
	{
		//�������һ�£����ж�Ϊ�����˳�ʼXML
		if (fisrtUsageDate == "1995.09.02")
		{
			QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��⵽���ܵ���Կ������Ϊ��"));
			if (!base64.Encrypt(filePath))
			{
				qDebug() << QStringLiteral("�������ļ�������ʧ��!");
				return -5;
			}
			return -2;
		}
		else//�����һ��
		{
			if (iViableDays > 0 && iViableTimes >= 0)
			{
				//����ʱ����
				QStringList temp = fisrtUsageDate.split(".");
				int iyearBefor = temp.at(0).toInt();
				int imonthBefor = temp.at(1).toInt();
				int idayBefor = temp.at(2).toInt();

				int iDateBefore = cal(iyearBefor, imonthBefor, idayBefor);
				int iDateAfter = cal(iYear, iMonth, iDay);

				//���ʱ���Ƿ��޸�
				if (iDateBefore > iDateAfter)
				{
					QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��⵽ϵͳʱ�䱻�ı䣡"));
					if (!base64.Encrypt(filePath))
					{
						qDebug() << QStringLiteral("�������ļ�������ʧ��!");
						return -5;
					}
					return -3;
				}

				//��������
				int iGap = iDateAfter - iDateBefore;
				iViableDays = iViableDays - iGap;
				if (iViableDays < 0)
				{
					msg = QString::fromLocal8Bit("��������ѽ���������ϵ��Ӧ��ע�ᣡ");
					QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), msg);

					if (!base64.Encrypt(filePath))
					{
						qDebug() << QStringLiteral("�������ļ�������ʧ��!");
						return -5;
					}
					return -4;
				}

				XmlTools::GetInstance()->SetValue("UsageConfig/viableDays", QString::number(iViableDays).toStdString().c_str());
				XmlTools::GetInstance()->SetValue("UsageConfig/viableTimes", QString::number(iViableTimes).toStdString().c_str());

				msg = QString::fromLocal8Bit("%1�պ��24:00ʱ������%2�κ����ý�����").arg(iViableDays).arg(iViableTimes);
				QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), msg);

				if (!base64.Encrypt(filePath))
				{
					qDebug() << QStringLiteral("�������ļ�������ʧ��!");
					return -5;
				}
				return 0;
			}
			else
			{
				msg = QString::fromLocal8Bit("��������ѽ���������ϵ��Ӧ��ע�ᣡ");
				QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), msg);

				if (!base64.Encrypt(filePath))
				{
					qDebug() << QStringLiteral("�������ļ�������ʧ��!");
					return -5;
				}
				return -4;
			}
		}
	}
	//�����������ͬһ���������µ���Կ�ļ�Ҫ�����ע������״����д���
	else
	{
		//д���״��������ڣ�����xml���ڼ�����
		msg = QString::fromLocal8Bit("%1�պ��24:00ʱ������%2�κ����ý�����").arg(iViableDays).arg(iViableTimes);
		QMessageBox::critical(0, QString::fromLocal8Bit("��ʾ"), msg);
		firstRun(CDKey, iViableDays, iViableTimes, date);

		if (!base64.Encrypt(filePath))
		{
			qDebug() << QStringLiteral("�������ļ�������ʧ��!");
			return -5;
		}
		return 0;
	}
}

void QVerification::firstRun(QString CDKey, int iDays, int iTimes, QString date)
{
	//ɾ����ʷ��Ϣ
	removeRegedit("cdKey");
	removeRegedit("firstRunDate");

	XmlTools::GetInstance()->SetValue("UsageConfig/fisrtUsageDate", date.toLocal8Bit().constData());
	XmlTools::GetInstance()->SetValue("UsageConfig/viableDays", QString::number(iDays).toStdString().c_str());
	XmlTools::GetInstance()->SetValue("UsageConfig/viableTimes", QString::number(iTimes).toStdString().c_str());

	//32λ����������WOW64ģʽ�£�setting���洢�� HKEY_LOCAL_MACHINE\Software\WOW6432Node��
	writeRegedit("firstRunDate", date);
	writeRegedit("cdKey", CDKey);
}
