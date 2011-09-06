//////////////////////////////////
//
// Ilkka Prusi, 2011
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QModelIndex>
#include <QtMultimedia/QAudioOutput>

#include "AudioFile.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
	void FileSelection(QString szFile);
	
private slots:
	void onFileSelected(QString szFile);
	
	void onAudioState(QAudio::State enState);
	void onPlayNotify();
	
	void on_actionFile_triggered();
	void on_actionPlay_triggered();
	void on_actionStop_triggered();
	
    void on_listWidget_doubleClicked(const QModelIndex &index);
	
	void on_actionAbout_triggered();
	

protected:
    void dumpDeviceFormat(QAudioDeviceInfo info);
    
private:
    Ui::MainWindow *ui;
	AudioFile *m_pAudioFile;
	QAudioOutput *m_pAudioOut;
	QIODevice *m_pDevOut;
	
	// temp, push-mode
	qint64 m_nWritten;
	char *m_pSampleData;
	qint64 m_nSampleSize;
};

#endif // MAINWINDOW_H
