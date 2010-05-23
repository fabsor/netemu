#ifndef BORING_GAME_H
#define BORING_GAME_H

#include <QtGui/QMainWindow>
#include "ui_boring_game.h"
class boring_game : public QMainWindow
{
    Q_OBJECT

public:
    boring_game(QWidget *parent = 0);
    ~boring_game();
private slots:
	void hostGame();
	void joinGame();

private:
    Ui::boring_gameClass ui;
    struct netemu_p2p_connection *p2p;
};

#endif // BORING_GAME_H
