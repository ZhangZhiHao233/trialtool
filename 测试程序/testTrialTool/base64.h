#pragma once

#include <QObject>

class Base64 : public QObject
{
	Q_OBJECT

public:
	Base64(QObject *parent = NULL);
	~Base64();

public:
	//�����ļ���������ԭ�ļ�
	bool Encrypt(QString fullPathName);

	//�����ļ���������ԭ�ļ�
	bool Decrypt(QString fullPathName);
};
