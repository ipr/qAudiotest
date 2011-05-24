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

	ui->horizontalSlider->hide();
	
	// if file given in command line
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() > 1)
	{
		ui->listWidget->addItem(new QListWidgetItem(vCmdLine[1]));
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
		ui->statusBar->showMessage("Failed to determine file type");
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
		ui->statusBar->showMessage("Unknown/unsupported file type");
		return;
	}
	
	if (m_pAudioFile->ParseFile(szFilename) == false)
	{
		// failed opening/processing, not supported type?
		ui->statusBar->showMessage("Failure reading file");
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
	
	if (format.isValid() == false)
	{
		ui->statusBar->showMessage("Invalid audio-format");
		return;
	}

	double nFrame = ( format.channels() * format.sampleSize() / 8 );
    double usInBuffer = ( (m_pAudioFile->sampleDataSize()*1000000ui64) / nFrame ) / format.frequency();
	double dBuf = nFrame * format.frequency(); // should be size in bytes for one second
	ui->horizontalSlider->setMaximum(usInBuffer/1000); // -> msec
	ui->horizontalSlider->setMinimum(0);
	ui->horizontalSlider->setValue(0);

	m_nWritten = 0;
	m_pSampleData = (char*)m_pAudioFile->sampleData();
	m_nSampleSize = m_pAudioFile->sampleDataSize();
	
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (info.isFormatSupported(format) == false) 
	{
		ui->statusBar->showMessage("Unsupported audio-format");
		return;
	}
	
	m_pAudioOut = new QAudioOutput(format, this);
	connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
	connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));
	
	// test, increase buffering for smoother playback
	// on IO-load (Windows trashes disk often.. keep more in play-buffer)
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
	// to reduce problems on heavy loads (buffer exhaustion)
	QThread *pCur = QThread::currentThread();
	pCur->setPriority(QThread::TimeCriticalPriority);
	
	// write initial
	qint64 nWritten = m_pDevOut->write(m_pSampleData, m_nSampleSize);
	if (nWritten == -1)
	{
		on_actionStop_triggered();
		return;
	}
	m_nWritten += nWritten;
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
	// test: only write on intervals (when some buffer has been consumed)
	// to reduce CPU-load (no need write when buffer is nearly full..)
	qint64 nWritten = m_pDevOut->write(m_pSampleData + m_nWritten, m_nSampleSize - m_nWritten);
	if (nWritten == -1)
	{
		on_actionStop_triggered();
		return;
	}
	m_nWritten += nWritten;
	
	// TODO: catch user positioning of slider..
	int iValue = ui->horizontalSlider->value();
	ui->horizontalSlider->setValue(iValue + 250);
}

void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		// TODO:
		// if already playing sound -> just add to list
		// otherwise, start playback

		// (temp)
		ui->listWidget->addItem(new QListWidgetItem(szFile));
		
		// TODO: check audio-state instead?
		if (m_pAudioOut == nullptr)
		{
			emit FileSelection(szFile);
		}
	}
}

void MainWindow::on_actionPlay_triggered()
{
	QListWidgetItem *pItem = ui->listWidget->currentItem();
	if (pItem == nullptr)
	{
		return;
	}
	
	// TODO: check audio-state instead?
	if (m_pAudioOut != nullptr)
	{
		// already playing?
		//return;
		on_actionStop_triggered();
	}
	
	emit FileSelection(pItem->text());
	
	// TODO:
	// if not already playing,
	// get current selection in list
	// and start playback
	//emit FileSelection(szFile);
	
	/*
	if (m_pAudioOut != nullptr)
	{
		on_actionStop_triggered();
	}
	*/
    
}

void MainWindow::on_actionStop_triggered()
{
	// TODO: deleted automatically by output-device?
	m_pDevOut = nullptr;
	
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
