#include <stdlib.h>
#include <math.h>

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "microphone.h"

#define BUFFER_SIZE 4096

AudioInfo::AudioInfo(QObject *parent, QAudioInput *device)
    :QIODevice(parent)
{
    input = device;

    m_maxValue = 0;
}

AudioInfo::~AudioInfo()
{

}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    //qDebug()<<"data:"<<data;

    int samples = len/2; // 2 bytes per sample
    int maxAmp = 32768; // max for S16 samples
    bool clipping = false;

    m_maxValue = 0;

    qint16 *s = (qint16*)data;

    // sample format is S16LE, only!

    for (int i = 0; i < samples; ++i) {
        qint16 sample = *s;
        s++;
        if (abs(sample) > m_maxValue) m_maxValue = abs(sample);
    }
    // check for clipping
    if (m_maxValue >= (maxAmp - 1))
        clipping = true;

    float value = ((float)m_maxValue/(float)maxAmp);
    if (clipping)
        m_maxValue = 100;
    else
        m_maxValue = (int)(value*100);

    ses=data;

    if(m_maxValue>0)
        emit update();

    return len;
}

int AudioInfo::LinearMax()
{
    return m_maxValue;
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    level = 0;
    setMinimumHeight(10);
    setMinimumWidth(100);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    if (level == 0)
        return;
    QPainter painter(this);
    painter.setPen(Qt::red);

    int pos = ((painter.viewport().right())-(painter.viewport().left()))*level/100;
    for (int i = 0; i < 10; ++i) {
        int x1 = painter.viewport().left();
        int y1 = painter.viewport().top()+i;
        int x2 = painter.viewport().left()+pos;
        int y2 = painter.viewport().top()+i;
        if (x2 < painter.viewport().left())
            x2 = painter.viewport().left();

        painter.drawLine(QPoint(x1, y1),QPoint(x2, y2));
    }
}

void RenderArea::setLevel(int value)
{
    level = value;
    update();
}


Microphone::Microphone(QWidget *parent):QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;

    canvas = new RenderArea;
    layout->addWidget(canvas);

    deviceBox = new QComboBox(this);
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i)
        deviceBox->addItem(devices.at(i).deviceName(), qVariantFromValue(devices.at(i)));

    connect(deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button2 = new QPushButton(this);
    button2->setText(tr("Pause Sound"));
    connect(button2, SIGNAL(clicked()), SLOT(toggleSuspend()));
    layout->addWidget(button2);

    this->setMaximumHeight(70);
    this->setLayout(layout);
    this->show();

    buffer = new char[BUFFER_SIZE];

    //pullMode = true;

    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    /*QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }*/

    if(format.sampleSize() != 16) {
        qWarning()<<"audio device doesn't support 16 bit samples, example cannot run";
        audioInput = 0;
        button->setDisabled(true);
        button2->setDisabled(true);
        return;
    }

    audioInput = new QAudioInput(format,this);
    connect(audioInput, SIGNAL(notify()), SLOT(status()));
    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(state(QAudio::State)));
    audioinfo  = new AudioInfo(this,audioInput);
    connect(audioinfo, SIGNAL(update()), SLOT(refreshDisplay()));
    audioinfo->start();
    audioInput->start(audioinfo);
}

Microphone::~Microphone() {}

void Microphone::status()
{
    //qWarning()<<"bytesReady = "<<audioInput->bytesReady()<<" bytes, elapsedUSecs = "<<audioInput->elapsedUSecs()<<", processedUSecs = "<<audioInput->processedUSecs();
}

void Microphone::toggleSuspend()
{
    // toggle suspend/resume
    if(audioInput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        audioInput->resume();
        button2->setText("Pause Sound");
    } else if (audioInput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        audioInput->suspend();
        button2->setText("Record Sound");
    } else if (audioInput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        audioInput->resume();
        button2->setText("Pause Sound");
    } else if (audioInput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void Microphone::state(QAudio::State state)
{
    qWarning() << " state=" << state;
}

void Microphone::refreshDisplay()
{
    canvas->setLevel(audioinfo->LinearMax());
}

void Microphone::deviceChanged(int idx)
{
    audioinfo->stop();
    audioInput->stop();
    audioInput->disconnect(this);
    delete audioInput;

    device = deviceBox->itemData(idx).value<QAudioDeviceInfo>();
    audioInput = new QAudioInput(device, format, this);
    connect(audioInput, SIGNAL(notify()), SLOT(status()));
    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(state(QAudio::State)));
    audioinfo->start();
    audioInput->start(audioinfo);
}
