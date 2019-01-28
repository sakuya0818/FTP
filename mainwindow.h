#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QFileDialog>
#include <QMetaEnum>
#include <QUrl>
#include <QFile>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    bool initFTP();

    void replyFinished(QNetworkReply*);

    void loadProgress(qint64 bytesSent, qint64 bytesTotal);

    QString getFileName(QString m_filePath);

    void readContent();

    void replyError(QNetworkReply::NetworkError error);

private slots:
    void on_Btn_upload_clicked();

    void on_Btn_download_clicked();

private:
    Ui::MainWindow *ui;

    QNetworkAccessManager *accessManager;

    QString ftpPath;
    QString user;
    QString password;

    QFile *file;

    QNetworkReply *reply;
};

#endif // MAINWINDOW_H
