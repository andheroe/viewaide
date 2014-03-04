#include "regandlogin.h"
#include "connectwithserver.h"
#include "ui_regandlogin.h"

RegAndLogIn::RegAndLogIn(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegAndLogIn)
{
    ConnectWithServer *con;
    con=new ConnectWithServer;

    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    connect(this,SIGNAL(netIsNotWorking()),this,SLOT(hideLogin()));
    connect(this, SIGNAL(windowCreate()), SLOT(isAccCreate()), Qt::QueuedConnection);
    connect(this,SIGNAL(fileWasCreate()),this,SLOT(inputToApp()));
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
    ui->pushButton_6->hide();
    ui->pushButton_7->hide();
    ui->pushButton_8->hide();
    ui->lineEdit_3->hide();
    ui->lineEdit_4->hide();
    ui->lineEdit_5->hide();
    ui->lineEdit_6->hide();
    ui->checkBox->hide();
    ui->label->hide();
    ui->label_2->hide();
    this->setFixedSize(318,475);
    this->setWindowTitle("Viewaide");
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor::fromRgb(45,64,71));
    this->setPalette(pal);
    QPixmap logomap(":/res/logo_login.png");
    ui->logo->setPixmap(logomap);
    QRegExp firLasName("^[A-Z a-z а-я А-Я]{1,30}$");
    QValidator *firLasNameValidator = new QRegExpValidator(firLasName, this);
    ui->lineEdit_3->setValidator(firLasNameValidator);
    ui->lineEdit_4->setValidator(firLasNameValidator);
    QRegExp emailRX("^[a-z \\. \\@ \\- \\_ 0-9]{1,256}$");
    QValidator *emailValidator = new QRegExpValidator(emailRX, this);
    ui->lineEdit->setValidator(emailValidator);
    ui->lineEdit_5->setValidator(emailValidator);
    if(con->netIsWorking()==false)
    {
        emit netIsNotWorking();
    }
    delete con;
}

RegAndLogIn::~RegAndLogIn()
{
    delete ui;
}

void RegAndLogIn::on_pushButton_2_clicked()
{
    ui->lineEdit->hide();
    ui->lineEdit->setText("");
    ui->lineEdit->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->lineEdit_2->hide();
    ui->lineEdit_2->setText("");
    ui->lineEdit_2->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->pushButton->hide(); 
    ui->pushButton_2->hide();
    ui->pushButton_3->hide();
    ui->label->hide();
    ui->pushButton_4->show();
    ui->pushButton_5->show();
    ui->pushButton_6->show();
    ui->pushButton_8->show();
    ui->lineEdit_3->show();
    ui->lineEdit_4->show();
    ui->lineEdit_5->show();
    ui->lineEdit_6->show();
    ui->checkBox->show();
}

void RegAndLogIn::on_pushButton_4_clicked()
{
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
    ui->pushButton_6->hide();
    ui->pushButton_8->hide();
    ui->lineEdit_3->hide();
    ui->lineEdit_3->setText("");
    ui->lineEdit_3->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->lineEdit_4->hide();
    ui->lineEdit_4->setText("");
    ui->lineEdit_4->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->lineEdit_5->hide();
    ui->lineEdit_5->setText("");
    ui->lineEdit_5->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->lineEdit_6->hide();
    ui->lineEdit_6->setText("");
    ui->lineEdit_6->setStyleSheet("background-color:rgb(255,255,255);"
                                "border: 2px solid rgb(17,164,192);"
                                "border-radius: 5px;padding: 0 4px;");
    ui->checkBox->hide();
    ui->label_2->hide();
    ui->lineEdit->show();
    ui->lineEdit_2->show();
    ui->pushButton->show();
    ui->pushButton_2->show();
    ui->pushButton_3->show();
}

void RegAndLogIn::on_pushButton_clicked()
{
    ui->label->setText("");
    ui->label->hide();
    bool empty(false);
    if(ui->lineEdit->text()=="")
    {
        empty=true;
        ui->lineEdit->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    if(ui->lineEdit_2->text()=="")
    {
        empty=true;
        ui->lineEdit_2->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    if (empty)
        return;
    ConnectWithServer *logToServer;
    logToServer=new ConnectWithServer;
    connect(logToServer,SIGNAL(notConnectWithServer()),this,SLOT(hideLogin()));
    connect(logToServer,SIGNAL(trueLogin()),this,SLOT(inputToApp()));
    connect(logToServer,SIGNAL(falseLogin()),this,SLOT(notInputToApp()));
    logToServer->login(ui->lineEdit->text(),ui->lineEdit_2->text());
    QFile fileAccount("account.txt");
    fileAccount.open(QIODevice::WriteOnly);
    QTextStream out(&fileAccount);
    out << ui->lineEdit->text();
    fileAccount.close();
    delete logToServer;
}

void RegAndLogIn::on_pushButton_5_clicked()
{
    ui->label_2->setText("");
    ui->label_2->hide();
    bool empty(false);
    if(ui->lineEdit_6->text()=="")
    {
        empty=true;
        ui->lineEdit_6->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    if(ui->lineEdit_3->text()=="")
    {
        empty=true;
        ui->lineEdit_3->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    else
        ui->lineEdit_3->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    if(ui->lineEdit_4->text()=="")
    {
        empty=true;
        ui->lineEdit_4->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    else
        ui->lineEdit_4->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    if(ui->lineEdit_5->text()=="")
    {
        empty=true;
        ui->lineEdit_5->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    }
    if (empty)
        return;
    ConnectWithServer *regToServer;
    regToServer=new ConnectWithServer;
    connect(regToServer,SIGNAL(notConnectWithServer()),this,SLOT(hideLogin()));
    connect(regToServer,SIGNAL(trueReg()),this,SLOT(dataAcceptedOnServ()));
    connect(regToServer,SIGNAL(falseReg()),this,SLOT(dataNotAcceptedOnServ()));
    regToServer->registration(ui->lineEdit_3->text(),ui->lineEdit_4->text(),
                              ui->lineEdit_5->text(),ui->lineEdit_6->text());
    delete regToServer;
}

void RegAndLogIn::on_lineEdit_3_textChanged(const QString &arg1)
{
    QString str;
    str=arg1;
    str.truncate(arg1.length()-1);
    if(arg1.length()==1)
    {
        str=arg1.toUpper();
        ui->lineEdit_3->setText(str);
    }
    if(arg1.length()>1)
    {
        QChar ch;
        ch=arg1.at(arg1.length()-1);
        str.append(ch.toLower());
        ui->lineEdit_3->setText(str);
    }
}

void RegAndLogIn::on_lineEdit_4_textChanged(const QString &arg1)
{
    QString str;
    str=arg1;
    str.truncate(arg1.length()-1);
    if(arg1.length()==1)
    {
        str=arg1.toUpper();
        ui->lineEdit_4->setText(str);
    }
    if(arg1.length()>1)
    {
        QChar ch;
        ch=arg1.at(arg1.length()-1);
        str.append(ch.toLower());
        ui->lineEdit_4->setText(str);
    }
}

void RegAndLogIn::on_lineEdit_editingFinished()
{
    QString str(ui->lineEdit->text());
    int pos(-1),countD(0);
    bool check(false);
    for(int i=0;i<str.length();i++)
        if(str.at(i)=='@')
        {
            pos=i;
            countD++;
        }
    if(countD>1)
    {
        ui->lineEdit_5->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
        return;
    }
    if(pos<1)
        ui->lineEdit->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    else
    {
        check=true;
        for(int i=pos+1;i<str.length();i++)
            if((str.at(i)=='.')&&((i==pos+1)||(i==str.length()-1)||(str.at(i-1)=='.')||(str.at(i+1)=='.')))
                check=false;
        for(int i=0;i<pos;i++)
            if((str.at(i)=='.')&&((i==0)||(i==pos-1)||(str.at(i-1)=='.')||(str.at(i+1)=='.')))
                check=false;
        if(check==false)
            ui->lineEdit->setStyleSheet("background-color:rgb(255,100,100);"
                                        "border: 2px solid rgb(17,164,192);"
                                        "border-radius: 5px;padding: 0 4px;");
        else
            ui->lineEdit->setStyleSheet("background-color:rgb(100,255,100);"
                                        "border: 2px solid rgb(17,164,192);"
                                        "border-radius: 5px;padding: 0 4px;");
    }
}

void RegAndLogIn::on_lineEdit_5_editingFinished()
{
    QString str(ui->lineEdit_5->text());
    int pos(-1),countD(0);
    bool check(false);
    for(int i=0;i<str.length();i++)
        if(str.at(i)=='@')
        {
            pos=i;
            countD++;
        }
    if(countD>1)
    {
        ui->lineEdit_5->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
        return;
    }
    if(pos<1)
        ui->lineEdit_5->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    else
    {
        check=true;
        for(int i=pos+1;i<str.length();i++)
            if((str.at(i)=='.')&&((i==pos+1)||(i==str.length()-1)||(str.at(i-1)=='.')||(str.at(i+1)=='.')))
                check=false;
        for(int i=0;i<pos;i++)
            if((str.at(i)=='.')&&((i==0)||(i==pos-1)||(str.at(i-1)=='.')||(str.at(i+1)=='.')))
                check=false;
        if(check==false)
            ui->lineEdit_5->setStyleSheet("background-color:rgb(255,100,100);"
                                        "border: 2px solid rgb(17,164,192);"
                                        "border-radius: 5px;padding: 0 4px;");
        else
            ui->lineEdit_5->setStyleSheet("background-color:rgb(100,255,100);"
                                        "border: 2px solid rgb(17,164,192);"
                                        "border-radius: 5px;padding: 0 4px;");
    }
}

void RegAndLogIn::on_lineEdit_2_editingFinished()
{
    QString leng(ui->lineEdit_2->text());
    if((leng.length()<6)&&(leng.length()>0))
        ui->lineEdit_2->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    if(leng.length()>5)
        ui->lineEdit_2->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
}

void RegAndLogIn::helpIn()
{
    QDesktopServices::openUrl(QUrl("http://viewaide.com/help"));
}

void RegAndLogIn::helpReg()
{
    QDesktopServices::openUrl(QUrl("http://viewaide.com/help"));
}

void RegAndLogIn::deal()
{
    QDesktopServices::openUrl(QUrl("http://viewaide.com/terms"));
}

void RegAndLogIn::isAccCreate()
{
    QFile accountFile("account.txt");
    if(accountFile.open(QIODevice::ReadOnly))
    {
        emit fileWasCreate();
    }
    accountFile.close();
}

void RegAndLogIn::inputToApp()
{
    hide();
    emit sigRunMainProgram();
    //Запуск приложения
}

void RegAndLogIn::notInputToApp()
{
    ui->label->setText("Неверный логин или пароль!");
    ui->label->show();
}

void RegAndLogIn::slotLogout()
{
//    QString path = QCoreApplication::applicationDirPath();
//    path += "//account.txt";
//    QFile file ( path );
//    if ( file.open(QIODevice::ReadOnly) )
//        file.remove();
    QFile file ( "account.txt" );
    file.remove();
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
}

void RegAndLogIn::dataAcceptedOnServ()
{
    ui->label_2->setText("Вы успешно зарегистрировались!\nНа почту было выслано письмо для подтверждения регистрации.");
    ui->label_2->show();
}

void RegAndLogIn::dataNotAcceptedOnServ()
{
    ui->label_2->setText("В ходе регистрации произошла ошибка!\nПроверьте введенные данные и повторите попытку!");
    ui->label_2->show();
}

void RegAndLogIn::hideLogin()
{
    ui->pushButton->hide();
    ui->pushButton_2->hide();
    ui->pushButton_3->hide();
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
    ui->label->setText("Нет соединения с сервером!\nПроверьте подключение!");
    ui->label->show();
    ui->pushButton_7->show();
}

void RegAndLogIn::on_pushButton_7_clicked()
{
    ConnectWithServer *rep;
    rep=new ConnectWithServer;
    if(rep->netIsWorking()==true)
    {
        ui->pushButton->show();
        ui->pushButton_2->show();
        ui->pushButton_3->show();
        ui->lineEdit->show();
        ui->lineEdit_2->show();
        ui->label->hide();
        ui->pushButton_7->hide();
    }
    delete rep;
}

void RegAndLogIn::on_lineEdit_6_editingFinished()
{
    QString leng(ui->lineEdit_6->text());
    if((leng.length()<6)&&(leng.length()>0))
        ui->lineEdit_6->setStyleSheet("background-color:rgb(255,100,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
    if((leng.length()>5))
        ui->lineEdit_6->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
}

void RegAndLogIn::on_lineEdit_3_editingFinished()
{
    QString leng(ui->lineEdit_3->text());
    if(leng.length()>0)
        ui->lineEdit_3->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
}

void RegAndLogIn::on_lineEdit_4_editingFinished()
{
    QString leng(ui->lineEdit_4->text());
    if(leng.length()>0)
        ui->lineEdit_4->setStyleSheet("background-color:rgb(100,255,100);"
                                    "border: 2px solid rgb(17,164,192);"
                                    "border-radius: 5px;padding: 0 4px;");
}
