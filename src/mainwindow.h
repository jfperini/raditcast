#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "bass.h"
#include "bassenc.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QTimer *Timer;  // Temporizador para el fader
     HRECORD rchan;
     void Dispositivos();
     void LoadConfig();
     void SaveConfig();


     DWORD Estado;
     HENCODE encoder;


 private slots:

 void Update();   //actualiza los tiempos
// void Start();
 void Stop();
 void Limpiar();
 void CurrentSong();
 void Reconectar();

 public slots:
  void Start();

};

#endif // MAINWINDOW_H
