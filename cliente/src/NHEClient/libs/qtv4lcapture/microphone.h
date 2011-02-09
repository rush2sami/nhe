#include <QPixmap>
#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QFile>
#include <QAudioInput>

class AudioInfo : public QIODevice
{
    Q_OBJECT
public:
    AudioInfo(QObject *parent, QAudioInput *device);
    ~AudioInfo();

    void start();
    void stop();

    int LinearMax();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    QAudioInput *input;

    const char *ses;

private:
    int m_maxValue;

signals:
    void update();
};


class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    void setLevel(int value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int level;
    QPixmap pixmap;
};

class Microphone : public QWidget
{
    Q_OBJECT
public:
    Microphone(QWidget *parent = 0);
    ~Microphone();
    AudioInfo *audioinfo;

private slots:
    void refreshDisplay();
    void status();
    //void readMore();
    //void toggleMode();
    void toggleSuspend();
    void state(QAudio::State s);
    void deviceChanged(int idx);

private:

    QAudioDeviceInfo device;
    QAudioFormat format;
    QAudioInput *audioInput;
    QIODevice *input;
    RenderArea *canvas;

    //bool pullMode;

    QPushButton *button;
    QPushButton *button2;
    QComboBox *deviceBox;

    char *buffer;
};

