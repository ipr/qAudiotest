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
            << "Capability"
            << "Value";
    /*
            << "Frequency"
            << "Sample rate"
            << "Sample size"
            << "Sample type"
            << "Codec";
            */
    
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

    ui->treeWidget->setColumnCount(headers.size());    
	ui->treeWidget->setHeaderLabels(headers);
    
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
    ui->treeWidget->expandAll();
}

void DeviceCaps::showDevice(QAudioDeviceInfo &info)
{
    //qDebug() << "device: " << info.deviceName();
    
    QTreeWidgetItem *pCapItem = nullptr;
    QTreeWidgetItem *pDevice = new QTreeWidgetItem((QTreeWidgetItem*)0);
    pDevice->setText(0, info.deviceName());
    ui->treeWidget->addTopLevelItem(pDevice);
    
    //qDebug() << "endianess:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "endianess");
    
    foreach (QAudioFormat::Endian end, info.supportedByteOrders())
    {
        //qDebug() << (int)end;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        if (end == QAudioFormat::BigEndian)
        {
            pValItem->setText(2, "BigEndian");
        }
        else
        {
            pValItem->setText(2, "LittleEndian");
        }
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    //qDebug() << "channel counts:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Channel count");
    
    foreach (int chcount, info.supportedChannelCounts())
    {
        //qDebug() << chcount;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(chcount));
        pCapItem->addChild(pValItem);
        
    }
    pDevice->addChild(pCapItem);

    //qDebug() << "frequencies:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Frequency");
    
    foreach (int frequ, info.supportedFrequencies())
    {
        //qDebug() << frequ;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(frequ));
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    //qDebug() << "sample rates:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample rate");
    
    foreach (int samplerate, info.supportedSampleRates())
    {
        //qDebug() << samplerate;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(samplerate));
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    //qDebug() << "sample sizes:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample size");
    
    foreach (int samplesize, info.supportedSampleSizes())
    {
        //qDebug() << samplesize;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(samplesize));
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    //qDebug() << "sample types:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample type");
    
    foreach (QAudioFormat::SampleType sampletype, info.supportedSampleTypes())
    {
        //qDebug() << (int)sampletype;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        switch (sampletype)
        {
        case QAudioFormat::Unknown:
            pValItem->setText(2, "Unknown");
            break;
        case QAudioFormat::SignedInt:
            pValItem->setText(2, "SignedInt");
            break;
        case QAudioFormat::UnSignedInt:
            pValItem->setText(2, "UnsignedInt");
            break;
        case QAudioFormat::Float:
            pValItem->setText(2, "Float");
            break;
        }
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    //qDebug() << "codecs:";
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Codec");
    
    foreach (QString codect, info.supportedCodecs())
    {
        //qDebug() << codect;
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, codect);
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    
}

