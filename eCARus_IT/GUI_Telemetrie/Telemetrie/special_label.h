#ifndef SPECIAL_LABEL_H
#define SPECIAL_LABEL_H

#include <QEvent>
#include <QWidget>
#include <QImage>
#include <QColor>
#include <QLabel>
#include <QMouseEvent>
#include <iostream>

/**
 * @brief The Special_Label class is able to handle Mouse Events by emitting individual signals for each mouse event
 */
class Special_Label : public QLabel
{
    Q_OBJECT

public:
    //IconEditor(QWidget *parent = 0);
    explicit Special_Label(QWidget *parent = 0);
    // bool event(QEvent *e);

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    ~Special_Label();
    void clicked();
    void released();
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
public slots:
    void mouseMoveEvent(QMouseEvent *);

signals:
    void mouse_leave();
    void mouse_enter();
    void mouse_move();
    void mouse_click();
    void mouse_release();

private:
    QString label_text;
    QString text_clicked;
};



#endif // Special_LABEL_H
