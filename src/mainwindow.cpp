
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"




bool err=false;


void CALLBACK EncoderNotify(HENCODE handle, DWORD status, void *user)
{
    if (status<0x10000) { // encoder/connection died
         err=true;
         return;
       }


}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


#ifdef Q_OS_WIN32
//setWindowFlags( Qt::Tool);
#endif

//evitamos que cambie el formulario
QSize fixedSize(this->width(),this->height());
setMinimumSize(fixedSize);
setMaximumSize(fixedSize);
setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

Timer = new QTimer();

connect(Timer, SIGNAL(timeout()), this, SLOT(Update())); // temporizaor horario
connect(ui->BtnPlay,SIGNAL(clicked()),this, SLOT(Start()));
connect(ui->BtnStop,SIGNAL(clicked()),this, SLOT(Stop()));

connect(ui->BtnLimpiar,SIGNAL(clicked()),this, SLOT(Limpiar()));





ui->EditPassword->setEchoMode(QLineEdit::Password);

Dispositivos();

LoadConfig();

//Start(); //auto start

if (ui->checkAutoPlay->isChecked())
    Start();



}

MainWindow::~MainWindow()
{
    SaveConfig();      // grabamos la configuracion
    delete ui;
}


//////////////////////////////////////////////////
  void MainWindow::Start(){


   BASS_RecordFree();
   int Dis = ui->ComboIn->currentIndex();
   BASS_RecordInit(Dis);

      // HENCODE encoder;
       char com[100];
       char *content;
       DWORD bitrate=128;

       QString Server,Password, Nombre, Url, Genero,Descripcion ;


       QString Bittmp;
       Bittmp=ui->ComboBirate->currentText();
       bitrate= Bittmp.toDouble();




       if(ui->RadioMp3->isChecked()){
           #ifdef Q_OS_WIN32
             sprintf(com,"encode/lame -r -s 44100 -b %d -",bitrate); // add "-x" for LAME versions pre-3.98
           #endif

           #ifdef Q_OS_UNIX
              sprintf(com,"lame -r -s 44100 -b %d -",bitrate); // add "-x" for LAME versions pre-3.98
           #endif

          content=BASS_ENCODE_TYPE_MP3;

         }


       if(ui->RadioOgg->isChecked()){

           #ifdef Q_OS_WIN32
            sprintf(com,"encode/oggenc -r -R 44100 -M %d -m %d -",bitrate,bitrate);
           #endif

           #ifdef Q_OS_UNIX
           sprintf(com,"oggenc -r -R 44100 -M %d -m %d -",bitrate,bitrate);
           #endif

          content=BASS_ENCODE_TYPE_OGG;

         }

      if(ui->RadioAac->isChecked()){

          char resto[]= " --silent --rawpcm  44100 2 16";


           #ifdef Q_OS_WIN32
             sprintf(com,"encode/enc_aacPlus - - --cbr %d000 %s", bitrate,resto);
              // sprintf(com,"encode/enc_aacPlus - - --cbr 128000 --silent --rawpcm  44100 2 16");

             // sprintf(com,"encode/aacplusenc - - 32");

           //sprintf(com,"encode/aacplusenc - - %d",bitrate);   //linux
           #endif


           #ifdef Q_OS_UNIX
               sprintf(com,"aacplusenc - - %d",bitrate);   //linux
           #endif


          content=BASS_ENCODE_TYPE_AAC;
         }


         //para instalar codec en linux
        // sudo apt-get install lame
        // sudo apt-get install vorbis-tools
         //sudo apt-get install aacplusenc




         Server=ui->EditServer->text();
         Password=ui->EditPassword->text();
         Nombre=ui->EditNombre->text();
         Url=ui->EditUrl->text();
         Genero=ui->EditGenero->text();
         Descripcion=ui->EditDescripcion->text();



     rchan=BASS_RecordStart(44100, 2, BASS_RECORD_PAUSE, 0, 0); // start recording

    /*  if(ui->RadioMp3->isChecked() || ui->RadioOgg->isChecked()){
         encoder=BASS_Encode_Start(rchan,com,BASS_ENCODE_NOHEAD|BASS_ENCODE_AUTOFREE,NULL,0);
        }

       if(ui->RadioAac->isChecked()){
          encoder=BASS_Encode_Start(rchan,"aacplusenc - - 32",BASS_ENCODE_AUTOFREE,0,0);  //aac+ para linux
         }*/


     // encoder=BASS_Encode_Start(rchan,com,BASS_ENCODE_NOHEAD|BASS_ENCODE_AUTOFREE,NULL,0);
      encoder=BASS_Encode_Start(rchan,com,BASS_ENCODE_AUTOFREE,0,0);


         qDebug() <<com;

      if (!BASS_Encode_CastInit(encoder,Server.toLatin1(),Password.toLatin1(),content,Nombre.toLatin1(),Url.toLatin1(),Genero.toLatin1(),Descripcion.toLatin1(),NULL,bitrate,true)) {

           qDebug()<<"Conexion erronea";
            BASS_ChannelStop(rchan);
           rchan=0;
  //           Reconectar();
            QTimer::singleShot(2000, this, SLOT(Reconectar()));

           return;
         }

      ui->TLog->append(tr("Encoding............ok  ")+ QTime::currentTime().toString());

         BASS_Encode_SetNotify(encoder,EncoderNotify,0); // notify of dead encoder/connection
         BASS_ChannelPlay(rchan,FALSE);
         ui->BtnPlay->setEnabled(false);


       //  BASS_Encode_CastSetTitle(encoder,"texto",0);
        CurrentSong();


  //Timer->start(20); // importante para el render
Timer->start(15); // importante para el render

SaveConfig();      // grabamos la configuracion


  }

  ///////////////////////////////////

  void MainWindow::Update(){


  int level;
  level=BASS_ChannelGetLevel(rchan);


 /* if (err){
         Timer->stop();
          this->Stop();
          err=false;
          qDebug() <<"Desconexion";
          Reconectar();

         }*/

      if (!ui->checkMonitor->isChecked())
           return;
  ui->Vu->setLeftValue(20*log(LOWORD(level)/100));
  ui->Vu->setRightValue(20*log(HIWORD(level)/100));



  }


//////////////////////////////////////////////////////////////////////
void MainWindow::Stop(){

    ui->BtnPlay->setEnabled(true);
    BASS_ChannelStop(rchan);
    rchan=0;
    Timer->stop();
    ui->Vu->setLeftValue(0);
    ui->Vu->setRightValue(0);
   // BOOL BASS_Encode_Stop(DWORD handle);
    ui->TLog->append(tr("Stop encoding"));

}
///////////////////////////////////////////////////////////////////////
 void MainWindow::Limpiar(){
  ui->TLog->clear();

}


/////////////////////////////////////////////////////////////////////
void MainWindow::Reconectar(){
    ui->TLog->append(tr("Offline....  ")+ QTime::currentTime().toString());


    ui->Vu->setLeftValue(0);
    ui->Vu->setRightValue(0);



    //Sleep(5000); // wait a sec

   ui->TLog->append(tr("Resetting the emission"));
   qApp->processEvents();

   this->Start();


}
//////////////////////////////////////////////////////////////
void MainWindow::CurrentSong(){




 QString texto;


 QString Path=QCoreApplication::applicationDirPath().toLatin1();
 QFile file(Path.toLatin1() + "/CurrentSound/CurrentSound.txt");
 file.open(QIODevice::ReadOnly| QIODevice::Text);
 QTextStream out(&file);


     // out << File.completeBaseName();

 QString line=out.readLine();


     QTimer::singleShot(30000, this, SLOT(CurrentSong()));
     // BASS_Encode_CastSetTitle(encoder,"Nueva version RaditCast " + QTime::currentTime().toString().toUtf8(),0);
      BASS_Encode_CastSetTitle(encoder,line.toUtf8(),0);
     // ui->TLog->append(line.toLatin1());
      ui->LabelCurrentSound->setText(line.toLatin1());
     // qDebug() << line.toLatin1();




      if (err){
             Timer->stop();
              this->Stop();
              err=false;
              qDebug() <<"Desconexion";
              Reconectar();
              return;

             }


}





////////////////////////////////////////////
void MainWindow::Dispositivos(){

    int a, count=0;
    BASS_DEVICEINFO info;

    for (a=0; BASS_RecordGetDeviceInfo(a, &info); a++){

        if (info.flags&BASS_DEVICE_ENABLED){  // device is enabled
            ui->ComboIn->addItem(QString::fromAscii(info.name));

            }

            count++; // count it
    }



    //for (int c=1;BASS_GetDeviceInfo(c,&i);c++) { // device 1 = el primer dispositivo
      //      if (i.flags&BASS_DEVICE_ENABLED) { // enabled, lo a�adimos...
        //        this->Dispositivo->addItem(nombre.fromAscii(i.name));
          //     }
        //}




}

/////////////////////////////////////////////////////////////////////////////////
 void  MainWindow::SaveConfig(){


     // guardamos la configuracion al salir


    QSettings RaditIni("Cast.ini", QSettings::IniFormat);



   RaditIni.setValue("Dispositivo",ui->ComboIn->currentIndex());

   RaditIni.setValue("Server",ui->EditServer->text());
   RaditIni.setValue("Password",ui->EditPassword->text());
   RaditIni.setValue("Nombre",ui->EditNombre->text());
   RaditIni.setValue("Url",ui->EditUrl->text());
   RaditIni.setValue("Genero",ui->EditGenero->text());
   RaditIni.setValue("Descripcion",ui->EditDescripcion->text());

   RaditIni.setValue("Mp3",ui->RadioMp3->isChecked());
   RaditIni.setValue("Ogg",ui->RadioOgg->isChecked());
   RaditIni.setValue("Aac",ui->RadioAac->isChecked());

   RaditIni.setValue("Birate",ui->ComboBirate->currentIndex());


   RaditIni.setValue("Monitor",ui->checkMonitor->isChecked());
   RaditIni.setValue("AutoPlay",ui->checkAutoPlay->isChecked());

  }




 /////////////////////////////////////////////////////////////////////////////////
  void  MainWindow::LoadConfig(){



     QSettings RaditIni("Cast.ini", QSettings::IniFormat);

  ui->ComboIn->setCurrentIndex(RaditIni.value("Dispositivo").toInt());

  ui->EditServer->setText(RaditIni.value("Server").toString());
  ui->EditPassword->setText(RaditIni.value("Password").toString());
  ui->EditNombre->setText(RaditIni.value("Nombre").toString());
  ui->EditUrl->setText(RaditIni.value("Url").toString());
  ui->EditGenero->setText(RaditIni.value("Genero").toString());
  ui->EditDescripcion->setText(RaditIni.value("Descripcion").toString());

  ui->RadioMp3->setChecked(RaditIni.value("Mp3").toBool());
  ui->RadioOgg->setChecked(RaditIni.value("Ogg").toBool());
  ui->RadioAac->setChecked(RaditIni.value("Aac").toBool());

  ui->ComboBirate->setCurrentIndex(RaditIni.value("Birate").toInt());

  ui->checkMonitor->setChecked(RaditIni.value("Monitor").toBool());
  ui->checkAutoPlay->setChecked(RaditIni.value("AutoPlay").toBool());



}




