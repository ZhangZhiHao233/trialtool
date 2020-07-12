#include "testtrialtool.h"
#include "qverification.h"

testTrialTool::testTrialTool(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QVerification qVerification;
	if (qVerification.verification() < 0)
	{
		::exit(-1);
	}
}

testTrialTool::~testTrialTool()
{

}

