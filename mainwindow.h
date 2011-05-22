#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
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
	
	
private:
    Ui::MainWindow *ui;
	AudioFile *m_pAudioFile;
	QAudioOutput *m_pAudioOut;
};

#endif // MAINWINDOW_H
