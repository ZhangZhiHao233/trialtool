#include "trialtool.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include "base64.h"

QString g_xmlpath;
Trialtool::Trialtool(QWidget *parent)
	: QWidget(parent),
	m_IsPermanent(0),
	m_iViableDays(0),
	m_iViableTimes(0)
{
	ui.setupUi(this);
	setFixedSize(this->width(), this->height());
	this->setWindowTitle(QString::fromLocal8Bit("试用权限注册机 v1.0"));

	connect(ui.spinBoxTimes, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedTimes(int)));
	connect(ui.spinBoxDays, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedDays(int)));
	connect(ui.pushButtonSet, SIGNAL(clicked()), this, SLOT(onSetTrial()));
	connect(ui.pushButtonPermanent, SIGNAL(clicked()), this, SLOT(onSetPermanent()));

	Base64 base64;
	QString filePath = QCoreApplication::applicationDirPath().append("/").append("usage.xml");
	//Base64解密
	if (!base64.Decrypt(filePath))
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("提示"), QStringLiteral("读配置文件，解密失败!"));
		qDebug()<<QStringLiteral("读配置文件，解密失败!");
		::exit(-1);
	}

	g_xmlpath = filePath;
	bool bRet = XmlTools::GetInstance()->LoadXmlFile(filePath.toLocal8Bit().constData());
	if (!bRet)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("加载秘钥文件失败！"));
		::exit(-1);
	}

	m_IsPermanent = QString(XmlTools::GetInstance()->GetValue("UsageConfig/IsPermanent")).trimmed().toInt();
	//m_fisrtUsageDate = QString(XmlTools::GetInstance()->GetValue("UsageConfig/fisrtUsageDate")).trimmed();
	m_iViableDays = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableDays")).trimmed().toInt();
	m_iViableTimes = QString(XmlTools::GetInstance()->GetValue("UsageConfig/viableTimes")).trimmed().toInt();

	if (m_IsPermanent)
	{
		ui.pushButtonPermanent->setText(QString::fromLocal8Bit("取消永久使用权"));
	}
	else
	{
		ui.pushButtonPermanent->setText(QString::fromLocal8Bit("授权永久使用"));
	}

	ui.spinBoxTimes->setValue(m_iViableTimes);
	ui.spinBoxDays->setValue(m_iViableDays);

	QString tips = QString::fromLocal8Bit("首次启动软件后%1日24:00时或启动%2次后将不可使用！").arg(m_iViableDays).arg(m_iViableTimes);
	ui.labelTip->setText(tips);
	QPalette pa;
	pa.setColor(QPalette::WindowText,Qt::red);
	ui.labelTip->setPalette(pa);
}

Trialtool::~Trialtool()
{
	if (!g_xmlpath.isNull())
	{
		//Base64加密
		Base64 base64;
		if (!base64.Encrypt(g_xmlpath))
		{
			QMessageBox::critical(0, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("读配置文件，加密失败!"));
		}
	}

	qDebug()<<QStringLiteral("未解密!");
}

void Trialtool::onValueChangedTimes(int times)
{
	QString tips = QString::fromLocal8Bit("首次启动软件后%1日24:00时或启动%2次后将不可使用！").arg(m_iViableDays).arg(times);
	ui.labelTip->setText(tips);
}

void Trialtool::onValueChangedDays(int days)
{
	QString tips = QString::fromLocal8Bit("首次启动软件后%1日24:00时或启动%2次后将不可使用！").arg(days).arg(m_iViableTimes);
	ui.labelTip->setText(tips);
}

void Trialtool::onSetTrial()
{
	int iTime = ui.spinBoxTimes->value();
	int iDays = ui.spinBoxDays->value();

	QString CDKey = getRandomString(10);
	qDebug()<<"CDKey"<<CDKey;
	XmlTools::GetInstance()->SetValue("UsageConfig/CDKey", CDKey.toStdString().c_str());
	XmlTools::GetInstance()->SetValue("UsageConfig/fisrtUsageDate", "1995.09.02");
	XmlTools::GetInstance()->SetValue("UsageConfig/viableDays", QString::number(iDays).toStdString().c_str());
	XmlTools::GetInstance()->SetValue("UsageConfig/viableTimes", QString::number(iTime).toStdString().c_str());


	QMessageBox mesg;
	mesg.warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("设置成功!"));
	close();
}

QString Trialtool::getRandomString(int length)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());

	const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int chrs_size = sizeof(chrs);

	char* ch = new char[length + 1];
	memset(ch, 0, length + 1);
	int randomx = 0;
	for (int i = 0; i < length; ++i)
	{
		randomx= rand() % (chrs_size - 1);
		ch[i] = chrs[randomx];
	}

	QString ret(ch);
	delete[] ch;
	return ret;
}

void Trialtool::onSetPermanent()
{
	QString msg;
	if (m_IsPermanent)
	{
		m_IsPermanent = false;
		ui.pushButtonPermanent->setText(QString::fromLocal8Bit("授权永久使用"));
		msg = QString::fromLocal8Bit("取消成功！");
	}
	else
	{
		m_IsPermanent = true;
		ui.pushButtonPermanent->setText(QString::fromLocal8Bit("取消永久使用权"));
		msg = QString::fromLocal8Bit("授权成功！");
	}

	QMessageBox::critical(0, QString::fromLocal8Bit("提示"), msg);
	XmlTools::GetInstance()->SetValue("UsageConfig/IsPermanent", QString::number(m_IsPermanent).toStdString().c_str());
	close();
}
