#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

#include "FileType.h"
#include "MemoryMappedFile.h"

#include "Iff8svx.h"
#include "IffAiff.h"
#include "RiffWave.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pAudioFile(nullptr),
	m_pAudioOut(nullptr)
{
    ui->setupUi(this);
	connect(this, SIGNAL(FileSelection(QString)), this, SLOT(onFileSelected(QString)));
	
	// on selection from list
	//connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, 

	// if file given in command line
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() > 1)
	{
		emit FileSelection(vCmdLine[1]);
	}
}

MainWindow::~MainWindow()
{
	if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
	}
	if (m_pAudioFile != nullptr)
	{
		delete m_pAudioFile;
	}
    delete ui;
}

void MainWindow::onFileSelected(QString szFile)
{
	//m_File.setFileName("C:/Tools/Dose by Mellow Chips.wav");

	if (m_pAudioOut != nullptr)
	{
		on_actionStop_triggered();
	}
	
	// TODO: change so that we can use same file again
	// without reopening..
	
	if (m_pAudioFile != nullptr)
	{
		// destruction must release resources (close file)
		delete m_pAudioFile;
		m_pAudioFile = nullptr;
	}
	
	CFileType Type;
	CMemoryMappedFile FileTmp;
	std::wstring szFilename = szFile.toStdWString();
	if (FileTmp.Create(szFilename.c_str()) == false)
	{
		return;
	}
	Type.DetermineFileType((uint8_t*)FileTmp.GetView(), 16);
	FileTmp.Destroy();
	
	if (Type.m_enFileType == HEADERTYPE_AIFF)
	{
		m_pAudioFile = new CIffAiff();
	}
	else if (Type.m_enFileType == HEADERTYPE_8SVX)
	{
		m_pAudioFile = new CIff8svx();
	}
	else if (Type.m_enFileType == HEADERTYPE_WAVE)
	{
		m_pAudioFile = new CRiffWave();
	}
	
	if (m_pAudioFile == nullptr)
	{
		// could not determine file type/not supported (yet)
		return;
	}
	
	if (m_pAudioFile->ParseFile(szFilename) == false)
	{
		// failed opening/processing, not supported type?
		return;
	}
	
	QAudioFormat format;
	if (m_pAudioFile->isBigEndian() == true)
	{
		format.setByteOrder(QAudioFormat::BigEndian);
	}
	else
	{
		format.setByteOrder(QAudioFormat::LittleEndian);
	}
	
	format.setCodec("audio/pcm");
	format.setFrequency(m_pAudioFile->sampleRate());
	format.setChannels(m_pAudioFile->channelCount());
	format.setSampleSize(m_pAudioFile->sampleSize());
	if (m_pAudioFile->sampleSize() <= 8)
	{
		format.setSampleType(QAudioFormat::UnSignedInt);
	}
	else
	{
		format.setSampleType(QAudioFormat::SignedInt);
	}
	
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (info.isFormatSupported(format)) 
	{
		m_pAudioOut = new QAudioOutput(format, this);
		connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
		//m_pAudioOut->start(m_pAudioFile);
		
		ui->horizontalSlider->setValue(0);
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

void MainWindow::onAudioState(QAudio::State)
{
}

void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		emit FileSelection(szFile);
	}
}

void MainWindow::on_actionPlay_triggered()
{
	/*
	if (m_pAudioOut != nullptr)
	{
		on_actionStop_triggered();
	}
	*/
    
	//m_pAudioOut->start(&m_File);
}

void MainWindow::on_actionStop_triggered()
{
	if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
		m_pAudioOut = nullptr;
	}
	
	if (m_pAudioFile != nullptr)
	{
		// destruction must release resources (close file)
		delete m_pAudioFile;
		m_pAudioFile = nullptr;
	}
}
