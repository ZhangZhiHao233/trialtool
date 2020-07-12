#include "base64.h"
#include <QDebug>
#include <QFile>

Base64::Base64(QObject *parent)
	: QObject(parent)
{

}

Base64::~Base64()
{

}

bool Base64::Encrypt(QString fullPathName)
{
	if (fullPathName.isEmpty())
	{
		return false;
	}

	QFile read_file(fullPathName);
	if (!read_file.open(QIODevice::ReadOnly))
	{
		qDebug() << QString::fromLocal8Bit("�ļ���ȡʧ��!");
		return false;
	}

	QByteArray encryption_Array = read_file.readAll().toBase64();

	QFile write_file(fullPathName);
	if (!write_file.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("�ļ�����ʧ��!");
		return false;
	}

	write_file.write(encryption_Array);

	read_file.close();
	write_file.close();

	qDebug() << QString::fromLocal8Bit("�ļ����ܳɹ�!");
	return true;
}

bool Base64::Decrypt(QString fullPathName)
{
	if (fullPathName.isEmpty())
	{
		return false;
	}

	QFile read_file(fullPathName);
	if (!read_file.open(QIODevice::ReadOnly))
	{
		qDebug() << QString::fromLocal8Bit("�ļ���ȡʧ��!");
		return false;
	}

	QByteArray decryption_Array = QByteArray::fromBase64(read_file.readAll());

	QFile write_file(fullPathName);
	if (!write_file.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("�ļ�����ʧ��!");
		return false;
	}
	write_file.write(decryption_Array);

	read_file.close();
	write_file.close();
	qDebug() << QString::fromLocal8Bit("�ļ����ܳɹ�!");
	return true;
}
