#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    connect(&ui->videoViever->thread,SIGNAL(finished()),SLOT(videoDidnotStart()));
    connect(ui->microphoneWidget->audioinfo,SIGNAL(update()),SLOT(soundGeldi()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWidget::on_btnExit_clicked()
{
    ui->videoViever->thread.stopUlan();
    qApp->exit(0);
}

void MainWidget::on_btnhooseOutFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Image"), "~", tr("Video Files (*.avi *.mpg *.mp4 *.mov *.mkv)"));
    if(!fileName.endsWith(".avi") &&
       !fileName.endsWith(".mpg") &&
       !fileName.endsWith(".mp4") &&
       !fileName.endsWith(".mov") &&
       !fileName.endsWith(".mkv")
        )
        fileName+=".mp4";

    ui->txtFileOut->setText(fileName);
}

void MainWidget::on_btnStartStopCapture_clicked()
{
    if(!ui->videoViever->thread.isRunning()){
        ui->btnStartStopCapture->setText(tr("Stop"));
        if(!ui->txtFileOut->text().isEmpty())
            ui->videoViever->setFileName(ui->txtFileOut->text());
            ui->videoViever->setDeviceName(ui->txtDevice->text());
            ui->videoViever->thread.startUlan();
    } else {
        ui->btnStartStopCapture->setText(tr("Start Capture"));
        ui->videoViever->thread.stopUlan();
    }
}

void MainWidget::videoDidnotStart(){
    ui->btnStartStopCapture->setText(tr("Start Capture"));
}

void MainWidget::soundGeldi(){
    if(ui->videoViever->thread.videoEncoder!=NULL && ui->microphoneWidget->audioinfo->ses!=NULL){
        ui->videoViever->thread.videoEncoder->encodeAudio(ui->microphoneWidget->audioinfo->ses);
        //qDebug()<<"sesgeldi:"<<ui->microphoneWidget->audioinfo->ses<<"\n";
       // qDebug("hahahahah");
    }
}
