/*
IOController-GUI - the GUI application for project IOController-FW
Copyright (C) 2017 Stepan Hamouz, s.hamouz@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <QKeyEvent>


static const char USB_DESCRIPTOR[] = "STMicroelectronics Virtual COM Port";
static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


/*
class keyEnterReceiver : public QWidget
{

Q_OBJECT
public:
    keyEnterReceiver(QWidget * parent = 0) {;}
    ~keyEnterReceiver() {;}


protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

bool keyEnterReceiver::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) )
        {
            return false;
        } else
        {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else
    {
        return QObject::eventFilter(obj, event);
    }
    return false;
}
*/


void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->type()==QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (key->key()==Qt::Key_0)
        {
          //return false;
        }
        else if (key->key()==Qt::Key_1)
        {
            if((lastState &0x01) == 0x01)
            {
              on_pushButtonControl0_ON_clicked(1);
            }
            else
            {
              on_pushButtonControl0_OFF_clicked(1);
            }
        }
        else if (key->key()==Qt::Key_2)
        {
            if((lastState &0x02) == 0x02)
            {
              on_pushButtonControl1_ON_clicked(1);
            }
            else
            {
              on_pushButtonControl1_OFF_clicked(1);
            }
        }
        else if (key->key()==Qt::Key_3)
        {
            if((lastState &0x04) == 0x04)
            {
              on_pushButtonControl2_ON_clicked();
            }
            else
            {
              on_pushButtonControl2_OFF_clicked();
            }
        }
        else if (key->key()==Qt::Key_4)
        {
            if((lastState &0x08) == 0x08)
            {
              on_pushButtonControl3_ON_clicked();
            }
            else
            {
              on_pushButtonControl3_OFF_clicked();
            }
        }
        else
        {
            //return QObject::eventFilter(obj, event);
        }
        //return true;
    } else
    {
        //return QObject::eventFilter(obj, event);
    }

    updateButtons();

    //return false;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isConnected(false),
    portFound(false),
    lastState(0)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    status = new QLabel;
    ui->statusBar->addWidget(status);

    fillPortsInfo();
    //on_pushButtonControl1_ON_clicked(true);  // Project dependant
    write("Configure 4, OUTPP, PPNN\r");  //OUTPP
    updateButtons();

    //keyEnterReceiver* key = new keyEnterReceiver();
   // ui->statusBar->installEventFilter(key);

}

MainWindow::~MainWindow()
{
  closeSerialPort();
  delete ui;
}

void MainWindow::fillPortsInfo()
{
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();

        if(0 == QString::compare(QString(USB_DESCRIPTOR), description, Qt::CaseInsensitive))
        {
          ComPorName = QString(info.portName());
          portFound = true;
          ui->label->setText(ComPorName);
        }
    }
}

bool MainWindow::openSerialPort()
{
  bool retVal = false;

  if(true == portFound)
  {
    serial->setPortName(ComPorName);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
       isConnected = true;
       retVal = true;

    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));

        isConnected = false;
    }
  }

  return retVal;
}

void MainWindow::closeSerialPort()
{
  if(true == portFound)
  {
    if (serial->isOpen())
    {
       serial->close();
       isConnected = false;
    }
  }
}

void MainWindow::on_pushButtonControl0_clicked(bool checked)
{
  (void) checked;
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::updateButtons()
{
  if(false == portFound)
  {
    ui->pushButtonControl0_OFF->setEnabled(false);
    ui->pushButtonControl0_ON->setEnabled(false);
    ui->pushButtonControl1_OFF->setEnabled(false);
    ui->pushButtonControl1_ON->setEnabled(false);
    ui->pushButtonControl2_OFF->setEnabled(false);
    ui->pushButtonControl2_ON->setEnabled(false);
    ui->pushButtonControl3_OFF->setEnabled(false);
    ui->pushButtonControl3_ON->setEnabled(false);
    ui->enDebugger->setEnabled(false);
  }
  else
  {
    uint8_t state = getState();
    lastState = state;

    ui->enDebugger->setEnabled(true);

    // button set 0
    if((state &0x01) == 0x01)
    {
      ui->pushButtonControl0_OFF->setEnabled(false);
      ui->pushButtonControl0_ON->setEnabled(true);
    }
    else
    {
      ui->pushButtonControl0_OFF->setEnabled(true);
      ui->pushButtonControl0_ON->setEnabled(false);
    }

    // button set 1
    if((state &0x02) == 0x02)
    {
      ui->pushButtonControl1_OFF->setEnabled(false);
      ui->pushButtonControl1_ON->setEnabled(true);
    }
    else
    {
      ui->pushButtonControl1_OFF->setEnabled(true);
      ui->pushButtonControl1_ON->setEnabled(false);
    }

    // button set 2
    if((state &0x04) == 0x04)
    {
      ui->pushButtonControl2_OFF->setEnabled(false);
      ui->pushButtonControl2_ON->setEnabled(true);
    }
    else
    {
      ui->pushButtonControl2_OFF->setEnabled(true);
      ui->pushButtonControl2_ON->setEnabled(false);
    }


    // button set 3
    if((state &0x08) == 0x08)
    {
      ui->pushButtonControl3_OFF->setEnabled(false);
      ui->pushButtonControl3_ON->setEnabled(true);
    }
    else
    {
      ui->pushButtonControl3_OFF->setEnabled(true);
      ui->pushButtonControl3_ON->setEnabled(false);
    }
  }
}

uint8_t MainWindow::getState()
{
  uint8_t returnVal = 0;

  QString response = write("Report binary\r");


  QRegularExpression re("Channels state 0x(\\w*)\r");
  QRegularExpressionMatch match = re.match(response);
  if (match.hasMatch())
  {
      QString matched = match.captured(1); //
      returnVal = matched.toUInt(NULL, 16);
  }

  return returnVal;
}

QString MainWindow::write(QString string)
{
  QString response;

  if(true == openSerialPort())
  {
    serial->write(string.toStdString().c_str());
    if (serial->waitForBytesWritten(5000))
    {
      // read response
      if (serial->waitForReadyRead(5000))
      {
        QByteArray responseData = serial->readAll();
        while (serial->waitForReadyRead(10))
        responseData += serial->readAll();

        response = QString(responseData);
        //emit this->response(response);
      }
      else
      {
        //emit timeout(tr("Wait read response timeout %1").arg(QTime::currentTime().toString()));
      }
    }
    else
    {
       //emit timeout(tr("Wait write request timeout %1").arg(QTime::currentTime().toString()));
    }

    showStatusMessage(response);
    closeSerialPort();
  }

  return response;
}

void MainWindow::on_pushButtonControl0_ON_clicked(bool checked)
{
  (void) checked;
  write("Disable 0\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl0_OFF_clicked(bool checked)
{
  (void) checked;
  write("Enable 0\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl1_ON_clicked(bool checked)
{
  (void) checked;
  write("Disable 1\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl1_OFF_clicked(bool checked)
{
  (void) checked;
  write("Enable 1\r");
  updateButtons();
}

void MainWindow::on_pushButton_clicked(bool checked)
{
  // refresh USB ports
  (void) checked;
  fillPortsInfo();
  updateButtons();
}

void MainWindow::on_enDebugger_clicked(bool checked)
{
   /*
  if(checked == true)
  {
    write("Configure 3, OUTPP, PPDOWN\r"); //PPDOWN   PPUP
    write("Disable 3\r");
  }
  else
  {
    write("Enable 3\r");
    write("Configure 3, IN, PPNO\r");
  }*/
}

void MainWindow::on_pushButtonControl2_ON_clicked()
{
  write("Disable 2\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl2_OFF_clicked()
{
  write("Enable 2\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl3_ON_clicked()
{
  write("Disable 3\r");
  updateButtons();
}

void MainWindow::on_pushButtonControl3_OFF_clicked()
{
  write("Enable 3\r");
  updateButtons();
}

void MainWindow::on_pushButton_2_clicked(bool checked)
{
   (void) checked;
   write("Configure 4, OUTPP, PPNN\r");  //OUTPP


}

void MainWindow::on_pushButton_2_clicked()
{

}
