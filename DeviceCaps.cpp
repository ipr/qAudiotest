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
    
    ui->treeWidget->setColumnCount(headers.size());    
	ui->treeWidget->setHeaderLabels(headers);
    
    showDeviceCaps();

    // catch when user selects a device to set it as current output..
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceSelection(int)));
}

DeviceCaps::~DeviceCaps()
{
    delete ui;
}

void DeviceCaps::deviceSelection(int dev)
{
    // TODO: something like this to get user selection of device?
    //emit selectedDevice(ui->comboBox->itemData(dev));
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
        
        // TODO: something like this to locate device after selection..?
        //ui->comboBox->setUserData(ui->comboBox->count(), info);
        
        // list device capabilities
        showDevice(info);
    }
    ui->treeWidget->expandAll();
}

void DeviceCaps::showDevice(QAudioDeviceInfo &info)
{
    QTreeWidgetItem *pCapItem = nullptr;
    QTreeWidgetItem *pDevice = new QTreeWidgetItem((QTreeWidgetItem*)0);
    pDevice->setText(0, info.deviceName());
    ui->treeWidget->addTopLevelItem(pDevice);
    
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "endianess");
    foreach (QAudioFormat::Endian end, info.supportedByteOrders())
    {
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
    
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Channel count");
    foreach (int chcount, info.supportedChannelCounts())
    {
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(chcount));
        pCapItem->addChild(pValItem);
        
    }
    pDevice->addChild(pCapItem);

    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample rate");
    foreach (int samplerate, info.supportedSampleRates())
    {
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(samplerate));
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample size");
    foreach (int samplesize, info.supportedSampleSizes())
    {
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, QString::number(samplesize));
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Sample type");
    foreach (QAudioFormat::SampleType sampletype, info.supportedSampleTypes())
    {
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
    
    pCapItem = new QTreeWidgetItem(pDevice);
    pCapItem->setText(1, "Codec");
    foreach (QString codect, info.supportedCodecs())
    {
        QTreeWidgetItem *pValItem = new QTreeWidgetItem(pCapItem);
        pValItem->setText(2, codect);
        pCapItem->addChild(pValItem);
    }
    pDevice->addChild(pCapItem);
    
}

