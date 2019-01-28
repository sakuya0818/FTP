#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置进度条
    ui->progressBar->setValue(0);
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // 对齐方式
    ui->progressBar->setFormat(QString::fromUtf8("当前无任务：%1%").arg(QString::number(0, 'f', 1)));

    //应用样式
    QFile stylesheet("main.qss");
    stylesheet.open(QFile::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());
    stylesheet.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************************************************
*函数名 : initFTP
*函数功能描述 : 初始化记录URL地址，用户名，密码，并检测URL地址的合法性
*函数参数 : 无
*函数返回值 : bool
*作者 : sakuya
*函数创作日期 : 2019/1/24
*函数修改日期 : 2019/1/28
*修改人 : sakuya
*修改原因 : 新增检测URL地址的合法性
*版本 : 1.1
*历史版本 : 1.0
***********************************************************************************************/
bool MainWindow::initFTP()
{
    //判断有没有填写URL地址和用户名、密码
    if (ui->lineEdit_ftp->text().isEmpty()) {
        QMessageBox::critical(NULL, tr("Error"), "URL地址不能为空");
        return false;
    }
    else if (ui->lineEdit_user->text().isEmpty()) {
        QMessageBox::critical(NULL, tr("Error"), "user不能为空");
        return false;
    }
    else if (ui->lineEdit_user->text().isEmpty()) {
        QMessageBox::critical(NULL, tr("Error"), "密码不能为空");
        return false;
    }
    else {
        ftpPath = ui->lineEdit_ftp->text();
        user = ui->lineEdit_user->text();
        password = ui->lineEdit_password->text();
        //检测URL地址是否合法
        QUrl url = QUrl(ftpPath);
        if (!url.isValid()) {
            QMessageBox::critical(NULL, tr("Error"), "URL地址不合法！");
            return false;
        }
        else if (url.scheme() != "ftp") {
            QMessageBox::critical(NULL, tr("Error"), "URL地址必须以ftp开头！");
            return false;
        }
        return true;
    }
}

/***********************************************************************************************
*函数名 : replyFinished
*函数功能描述 : 删除指针，更新和关闭文件
*函数参数 : 无
*函数返回值 : 无
*作者 : sakuya
*函数创作日期 : 2019/1/25
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
void MainWindow::replyFinished(QNetworkReply*)
{
    if (reply->error() == QNetworkReply::NoError) {
            reply->deleteLater();
            file->flush();
            file->close();
        }
        else {
            QMessageBox::critical(NULL, tr("Error"), "错误!!!");
        }
}

/***********************************************************************************************
*函数名 : loadProgress
*函数功能描述 : 更新进度条
*函数参数 : qint64
*函数返回值 : 无
*作者 : sakuya
*函数创作日期 : 2019/1/25
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
void MainWindow::loadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setValue(0);
    qDebug() << "loaded" << bytesSent << "of" << bytesTotal;
    ui->progressBar->setMaximum(bytesTotal); //最大值
    ui->progressBar->setValue(bytesSent);  //当前值
    double currentProgress = (bytesSent - ui->progressBar->minimum()) * 100.0 / (bytesTotal - ui->progressBar->minimum());
    ui->progressBar->setFormat(QString::fromUtf8("当前进度为：%1%").arg(QString::number(currentProgress, 'f', 1)));
}

/***********************************************************************************************
*函数名 : getFileName
*函数功能描述 : 获得传入路径的文件名
*函数参数 : QString
*函数返回值 : QString
*作者 : sakuya
*函数创作日期 : 2019/1/25
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
QString MainWindow::getFileName(QString m_filePath)
{
    QString temp;
    QString fileName;
    int count = -1;
    fileName = m_filePath;
    for(int i = 0; temp != "/"; i++)
    {
        temp = fileName.right(1);
        fileName.chop(1);
        count++;
    }
    fileName = m_filePath.right(count);

    return fileName;
}

/***********************************************************************************************
*函数名 : replyError
*函数功能描述 : 下载过程出错，进行报错处理（超时处理也是丢出超时信号，交由此槽函数进行处理）
*函数参数 : NetworkError
*函数返回值 : 无
*作者 : sakuya
*函数创作日期 : 2019/1/28
*函数修改日期 :
*修改人 :
*修改原因 :
*版本 : 1.0
*历史版本 : 无
***********************************************************************************************/
void MainWindow::replyError(QNetworkReply::NetworkError error)
{
    auto metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    //枚举值转换为字符串
    auto errStr = metaEnum.valueToKey(error);
    QMessageBox::critical(NULL, tr("Error"), QString(errStr));

    file->deleteLater();
    file = Q_NULLPTR;

    reply->deleteLater();
}

//下载时向本地文件中写入数据
void MainWindow::readContent()
{
    file->write(reply->readAll());
}

//上传文件
void MainWindow::on_Btn_upload_clicked()
{
    if (initFTP()) {
        //得到选择的文件的路径，保存在字符串链表中
        QStringList string_list;
        string_list = QFileDialog::getOpenFileNames(this, tr("选择文件"), "", tr("Files (*)"));
        if (!string_list.isEmpty()) {
            for (int i = 0; i < string_list.count(); i++) {
                QString filePath;
                filePath = string_list.at(i);
                file = new QFile(filePath);
                file->open(QIODevice::ReadOnly);
                QByteArray byte_file = file->readAll();

                QString fileName;
                fileName = getFileName(filePath);
                QString m_ftpPath;
                m_ftpPath = ftpPath + "/" + fileName;

                //把选中的文件上传到服务器
                accessManager = new QNetworkAccessManager(this);
                accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
                QUrl url(m_ftpPath);
                url.setPort(21);
                url.setUserName(user);
                url.setPassword(password);

                QNetworkRequest request(url);
                reply = accessManager->put(request, byte_file);

                connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
                connect(reply, SIGNAL(uploadProgress(qint64 ,qint64)), this, SLOT(loadProgress(qint64 ,qint64)));
                connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),SLOT(replyError(QNetworkReply::NetworkError)));
            }
        }
    }
}

//下载文件
void MainWindow::on_Btn_download_clicked()
{
    if (initFTP()) {
        QString folderPath;
        folderPath = QFileDialog::getExistingDirectory(this, tr("选择文件"), "", QFileDialog::ShowDirsOnly);
        file = new QFile(folderPath + "/test.jpg");
        file->open(QIODevice::WriteOnly);

        //从服务器上下载文件到选中文件夹
        QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
        accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
        QUrl url("ftp://192.168.43.129/software/timg.jpeg");
        url.setPort(21);
        url.setUserName("jinxiaodan");
        url.setPassword("abcd1234");

        QNetworkRequest request(url);
        reply = accessManager->get(request);

        connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(readContent()));
        connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
        connect(reply, SIGNAL(downloadProgress(qint64 ,qint64)), this, SLOT(loadProgress(qint64 ,qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),SLOT(replyError(QNetworkReply::NetworkError)));
    }
}
