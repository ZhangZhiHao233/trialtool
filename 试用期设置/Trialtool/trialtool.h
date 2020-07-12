#ifndef TRIALTOOL_H
#define TRIALTOOL_H

#include <QtWidgets/QWidget>
#include "ui_trialtool.h"
#include "XmlTools.h"

class Trialtool : public QWidget
{
	Q_OBJECT

public:
	Trialtool(QWidget *parent = 0);
	~Trialtool();

private:
	Ui::TrialtoolClass ui;
	bool m_IsPermanent;
	QString m_fisrtUsageDate;
	int m_iViableDays;
	int m_iViableTimes;

private:
	QString getRandomString(int length);

private slots:
	void onValueChangedTimes(int times);
	void onValueChangedDays(int days);
	void onSetTrial();
	void onSetPermanent();

};

#endif // TRIALTOOL_H
