#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QtMultimedia/QAudioOutput>

#include "wavfile.h"

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
	void on_actionFile_triggered();
	void onFileSelected(QString szFile);
	
private:
    Ui::MainWindow *ui;
	QFile m_File;
	QAudioOutput *m_pAudioOut;
	
	WavFile *m_pWavFile;
};

#endif // MAINWINDOW_H
