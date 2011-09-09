#include "DeviceCaps.h"
#include "ui_DeviceCaps.h"

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioDeviceInfo>


DeviceCaps::DeviceCaps(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceCaps)
{
    ui->setupUi(this);
    
    QStringList headers;
	headers << "Device"
            << "Byteorder"
            << "Channel count"
            << "Frequency"
            << "Sample rate"
            << "Sample size"
            << "Sample type"
            << "Codec";
    
    /*
    << "device: " << info.deviceName()
    << "byteorder: " << (int)format.byteOrder()
    << "channel count: " << format.channelCount()
    << "frequency: " << format.frequency()
    << "samplerate: " << format.sampleRate()
    << "samplesize: " << format.sampleSize()
    << "sampletype: " << (int)format.sampleType()
    << "codec: " << format.codec();
    */

    ui->tableWidget->setColumnCount(treeHeaders.size());    
	ui->tableWidget->setHeaderLabels(headers);
    
    showDeviceCaps();
}

DeviceCaps::~DeviceCaps()
{
    delete ui;
}

void DeviceCaps::showDeviceCaps()
{
    // enumerate devices
    // list to selection combo
    // list capabilities
    
    auto devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (QAudioDeviceInfo info, devices)
    {
        ui->comboBox->addItem(info.deviceName());
        
        // something like this to locate device after selection..?
        //ui->comboBox->setUserData(ui->comboBox->count(), info);
        
        // list device capabilities
        showDevice(info);
    }
    
}

void DeviceCaps::showDevice(QAudioDeviceInfo &info)
{
    qDebug() << "device: " << info.deviceName();
    
    qDebug() << "endianess:";
    foreach (QAudioFormat::Endian end, info.supportedByteOrders())
    {
        qDebug() << (int)end;
    }
    
    qDebug() << "channel counts:";
    foreach (int chcount, info.supportedChannelCounts())
    {
        qDebug() << chcount;
    }

    qDebug() << "frequencies:";
    foreach (int frequ, info.supportedFrequencies())
    {
        qDebug() << frequ;
    }
    
    qDebug() << "sample rates:";
    foreach (int samplerate, info.supportedSampleRates())
    {
        qDebug() << samplerate;
    }
    
    qDebug() << "sample sizes:";
    foreach (int samplesize, info.supportedSampleSizes())
    {
        qDebug() << samplesize;
    }
    
    qDebug() << "sample types:";
    foreach (int sampletype, info.supportedSampleTypes())
    {
        qDebug() << (int)sampletype;
    }
    
    qDebug() << "codecs:";
    foreach (QString codect, info.supportedCodecs())
    {
        qDebug() << codect;
    }
    
    
}

