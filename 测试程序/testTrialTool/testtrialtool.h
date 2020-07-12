#ifndef TESTTRIALTOOL_H
#define TESTTRIALTOOL_H

#include <QtWidgets/QWidget>
#include "ui_testtrialtool.h"

class testTrialTool : public QWidget
{
	Q_OBJECT

public:
	testTrialTool(QWidget *parent = 0);
	~testTrialTool();

private:
	Ui::testTrialToolClass ui;
};

#endif // TESTTRIALTOOL_H
