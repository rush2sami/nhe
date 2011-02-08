#include "nheclient.h"

NHEClient::NHEClient(QWidget *parent)
    : QMainWindow(parent)
{
	//this->nheAbout = new NHEAbout(this);
	ui.setupUi(this);
	connect(ui.actionSobre_QT,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
	//connect(ui.actionSobre_NHE,SIGNAL(triggered()),nheAbout,SLOT(show()));
}

NHEClient::~NHEClient()
{

}
