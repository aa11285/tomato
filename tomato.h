#ifndef TOMATO_H
#define TOMATO_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>

enum State {IDLE, FOCUS, REST, PAUSE};

namespace Ui {
class Tomato;
}

class Tomato : public QDialog
{
        Q_OBJECT

    public:
        explicit Tomato(QWidget *parent = 0);
        ~Tomato();

    protected:
        void closeEvent(QCloseEvent *event) override;

    private slots:
        void iconActivated(QSystemTrayIcon::ActivationReason reason);
        void iconMsgClicked();
        void updateTimer();

        void on_spinFocus_valueChanged(int arg1);
        void on_spinRest_valueChanged(int arg1);
        void on_buttonReset_clicked();


        void on_checkStartup_stateChanged(int arg1);

        void on_checkInfinity_stateChanged(int arg1);

    private:
        Ui::Tomato *ui;

        const int focusDefault = 25;
        const int restDefault = 5;

        void createActions();
        void createTrayIcon();
        void setIconState(State state);
        void startTomato();
        void pauseTomato();
        void cancelTomato();
        void continueTomato();
        void startRest();
        void verifyStartWithWindows();

        QAction *openSettingsAction;
        QAction *quitAction;

        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;

        QMenu *clickMenu;
        QAction *startAction;
        QAction *pauseAction;
        QAction *cancelAction;
        QAction *continueAction;

        int focusDuration;
        int restDuration;
        int elapsedSec;

        State state;
        QTimer* timer;
};

#endif // QT_NO_SYSTEMTRAYICON

#endif // TOMATO_H
