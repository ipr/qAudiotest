#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

// for testing
#include <QThread>

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
	
	// TODO: we may have 8-bit signed int..
	//if (m_pAudioFile->sampleSize() > 8)
	if (m_pAudioFile->isSigned() == true)
	{
		format.setSampleType(QAudioFormat::SignedInt);
	}
	else
	{
		format.setSampleType(QAudioFormat::UnSignedInt);
	}

	double nFrame = ( format.channels() * format.sampleSize() / 8 );
    double usInBuffer = ( (m_pAudioFile->sampleDataSize()*1000000ui64) / nFrame ) / format.frequency();
	double dBuf = nFrame * format.frequency(); 
	ui->horizontalSlider->setMaximum(usInBuffer/1000); // -> msec
	ui->horizontalSlider->setMinimum(0);
	ui->horizontalSlider->setValue(0);
	
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (info.isFormatSupported(format)) 
	{
		m_pAudioOut = new QAudioOutput(format, this);
		connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
		connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));

		// test, increase buffering for smoother playback
		// on IO-load (Windows trashes disk often..)
		int iBufSize = m_pAudioOut->bufferSize();
		if (iBufSize < dBuf)
		{
			m_pAudioOut->setBufferSize(dBuf);
		}
		
		// pull-mode (implement the interface needed in audio-file?)
		//m_pAudioOut->start(m_pAudioFile);

		m_pAudioOut->setNotifyInterval(250);
		
		// test: push-mode (need threading to work correctly)
		m_pDevOut = m_pAudioOut->start();
		
		// test, increase priority of playback-thread
		// to reduce problems on heavy loads
		QThread *pCur = QThread::currentThread();
		pCur->setPriority(QThread::TimeCriticalPriority);
		
		// TODO: make buffering of file anyway instead of memory-mapped?
		// experiencing "pauses" under IO-load on Windows..
		// or just make thread higher-priority?
		
		m_nWritten = 0;
		m_pSampleData = (char*)m_pAudioFile->sampleData();
		m_nSampleSize = m_pAudioFile->sampleDataSize();
		
		qint64 nWritten = m_pDevOut->write(m_pSampleData, m_nSampleSize);
		if (nWritten == -1)
		{
			return;
		}
		m_nWritten += nWritten;
	}
		
}

void MainWindow::onAudioState(QAudio::State enState)
{
	if (enState == QAudio::ActiveState)
	{
		ui->horizontalSlider->show();
	}
	else if (enState == QAudio::StoppedState)
	{
		// show that we stopped
		ui->horizontalSlider->hide();
	}
}

// triggered on certain intervals (set to output-device)
void MainWindow::onPlayNotify()
{
	// temp! testing only..
	// !! slow way
	//int iInterval = m_pAudioOut->notifyInterval();
	int iValue = ui->horizontalSlider->value();
	ui->horizontalSlider->setValue(iValue + 250);
	// /temp!
	
	// test: only write on intervals (when some buffer has been consumed)
	// to reduce CPU-load (no need write when buffer is nearly full..)
	qint64 nWritten = m_pDevOut->write(m_pSampleData + m_nWritten, m_nSampleSize - m_nWritten);
	if (nWritten == -1)
	{
		on_actionStop_triggered();
		return;
	}
	m_nWritten += nWritten;
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
