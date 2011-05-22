#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

#include "FileType.h"
#include "MemoryMappedFile.h"

#include "Iff8svx.h"
#include "RiffWave.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_pAudioOut(NULL)
{
    ui->setupUi(this);
	connect(this, SIGNAL(FileSelection(QString)), this, SLOT(onFileSelected(QString)));

	// if file given in command line
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() > 1)
	{
		emit FileSelection(vCmdLine[1]);
	}
	
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		emit FileSelection(szFile);
	}
}

void MainWindow::onFileSelected(QString szFile)
{
	//m_File.setFileName("C:/Tools/Dose by Mellow Chips.wav");
	
	CFileType Type;
	CMemoryMappedFile File;
	if (File.Create(szFile.toStdWString().c_str()) == false)
	{
		return;
	}
	Type.DetermineFileType(File.GetView(), 16);
	File.Destroy();
	
	if (Type.m_enFileType == HEADERTYPE_WAVE)
	{
		m_File.setFileName(szFile);
		m_File.open(QIODevice::ReadOnly);
		m_pWavFile = new WavFile();
		if (m_pWavFile->readHeader(m_File) == true)
		{
			QAudioFormat format = m_pWavFile->format();
			QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
			if (info.isFormatSupported(format)) 
			{
				m_pAudioOut = new QAudioOutput(format, this);
				m_pAudioOut->start(&m_File);
				
				ui->horizontalSlider->setValue(0);
			}
		}
	}
	else if (Type.m_enFileType == HEADERTYPE_8SVX)
	{
		// CIff8SVX File;
	}
		
/*	
	QAudioFormat format;
	// Set up the format, eg.
	format.setFrequency(8000);
	format.setChannels(1);
	format.setSampleSize(8);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);
   
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format)) 
	{
		qWarning()<<"raw audio format not supported by backend, cannot play audio.";
		return;
	}
   
	m_pAudioOut = new QAudioOutput(format, this);
	//connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), SLOT(finishedPlaying(QAudio::State)));
	m_pAudioOut->start(&m_File);
*/

}

