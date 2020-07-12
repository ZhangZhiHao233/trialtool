#pragma once

#include <QObject>

class Base64 : public QObject
{
	Q_OBJECT

public:
	Base64(QObject *parent = NULL);
	~Base64();

public:
	//加密文件，并覆盖原文件
	bool Encrypt(QString fullPathName);

	//解密文件，并覆盖原文件
	bool Decrypt(QString fullPathName);
};
