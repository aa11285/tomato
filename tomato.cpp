#include "tomato.h"
#include "ui_tomato.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCoreApplication>
#include <QMenu>
#include <QCloseEvent>
#include <QTimer>
#include <QTime>

#include <QToolTip>

#include <QtDebug>

Tomato::Tomato(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tomato)
{
    ui->setupUi(this);

    focusDuration = focusDefault;
    restDuration = restDefault;
    elapsedSec = 0;
    state = IDLE;

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &Tomato::iconActivated);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Tomato::iconMsgClicked);

    setIconState(IDLE);

    setWindowIcon(QIcon(":/sushi"));
    setWindowTitle(tr("Tomato です"));

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateTimer()));
}

Tomato::~Tomato()
{
    delete ui;
}

void Tomato::setIconState(State state)
{
    switch(state) {
    case IDLE:
        trayIcon->setIcon(QIcon(":/sushi"));
        trayIcon->show();
        break;
    case FOCUS:
        trayIcon->setIcon(QIcon(":/tomato"));
        trayIcon->show();
        break;
    case REST:
        trayIcon->setIcon(QIcon(":/coffee"));
        trayIcon->show();
        break;
    case PAUSE:
        trayIcon->setIcon(QIcon(":/pause"));
        trayIcon->show();
        break;
    default:
        ;
    }
}

void Tomato::closeEvent(QCloseEvent *event) // OVERRIDE
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void Tomato::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (QSystemTrayIcon::Trigger == reason) {
        clickMenu = new QMenu(this);
        clickMenu->move(cursor().pos().x(),
                        cursor().pos().y() - clickMenu->size().height() + 1);
        switch(state) {
            case IDLE: {
                clickMenu->addAction(startAction);
                clickMenu->show();
                break;
            }
            case FOCUS: {
                clickMenu->addAction(pauseAction);
                clickMenu->addAction(cancelAction);
                clickMenu->move(cursor().pos().x(),
                                cursor().pos().y() - clickMenu->size().height()*2 + 1);
                clickMenu->show();
                break;
            }
            case PAUSE: {
                clickMenu->addAction(continueAction);
                clickMenu->show();
                break;
            }
            case REST: {
                clickMenu->addAction(cancelAction);
                clickMenu->show();
                break;
            }
            default:
                ;
        }
    }

}

void Tomato::iconMsgClicked()
{
    qDebug() << "message clicked!";
}

void Tomato::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openSettingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Tomato::createActions()
{
    // context menu
    openSettingsAction = new QAction(tr("&Settings"), this);
    connect(openSettingsAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    // left-click menu
    startAction = new QAction(tr("&Start"), this);
    connect(startAction,&QAction::triggered, this,  &Tomato::startTomato);

    pauseAction = new QAction(tr("&Pause"), this);
    connect(pauseAction,&QAction::triggered, this,  &Tomato::pauseTomato);

    cancelAction = new QAction(tr("&Cancel"), this);
    connect(cancelAction,&QAction::triggered, this,  &Tomato::cancelTomato);

    continueAction = new QAction(tr("&Continue"), this);
    connect(continueAction,&QAction::triggered, this,  &Tomato::continueTomato);
}

void Tomato::startTomato()
{
    state = FOCUS;
    setIconState(state);
    timer->start(1000);
}

void Tomato::startRest()
{
    state = REST;
    setIconState(state);
    timer->start(1000);
}

void Tomato::pauseTomato()
{
    state = PAUSE;
    setIconState(state);
    timer->stop();
}

void Tomato::cancelTomato()
{
    if(FOCUS==state) {
        trayIcon->showMessage("Tomato",
                              tr("Cancelled at %1 min").arg(elapsedSec/60),
                              QIcon(":/coffee"),
                              1000);
    }
    timer->stop();
    elapsedSec = 0;
    state = IDLE;
    setIconState(IDLE);
}

void Tomato::continueTomato()
{
    state = FOCUS;
    setIconState(state);
    timer->start(1000);
}

void Tomato::on_spinFocus_valueChanged(int arg1)
{
    focusDuration = arg1;
}

void Tomato::on_spinRest_valueChanged(int arg1)
{
    restDuration = arg1;
}

void Tomato::on_buttonReset_clicked()
{
    focusDuration = focusDefault;
    restDuration = restDefault;
    ui->spinFocus->setValue(focusDuration);
    ui->spinRest->setValue(restDuration);
}

void Tomato::updateTimer()
{
    elapsedSec += 1;

    QTime currentTime(0,elapsedSec/60,elapsedSec%60,0);
    trayIcon->setToolTip(currentTime.toString("mm:ss"));

    // time's up
    if (FOCUS == state && elapsedSec/60 >= focusDuration) {
        timer->stop();
        elapsedSec = 0;
        trayIcon->showMessage("Tomato",
                              "Focus time's up!\nTake a rest~~",
                              QIcon(":/tomato"),
                              1000);
        state = REST;
        setIconState(REST);
        startRest();
    }
    else if (REST == state && elapsedSec/60 >= restDuration) {
        timer->stop();
        elapsedSec = 0;
        trayIcon->showMessage("Tomato",
                              "Rest time's up!\nNeed another round?",
                              QIcon(":/coffee"),
                              1000);
        state = IDLE;
        setIconState(IDLE);
    }
}

#endif
