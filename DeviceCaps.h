#ifndef DEVICECAPS_H
#define DEVICECAPS_H

#include <QDialog>

namespace Ui {
    class DeviceCaps;
}

class QAudioDeviceInfo;

class DeviceCaps : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceCaps(QWidget *parent = 0);
    ~DeviceCaps();

protected:
    void showDeviceCaps();
    void showDevice(QAudioDeviceInfo &info);
    
private:
    Ui::DeviceCaps *ui;
};

#endif // DEVICECAPS_H
