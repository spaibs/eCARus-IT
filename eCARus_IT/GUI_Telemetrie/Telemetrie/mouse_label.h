#ifndef MOUSE_LABEL_H
#define MOUSE_LABEL_H

#include <QEvent>
#include <QWidget>
#include <QImage>
#include <QColor>
#include <QLabel>
#include <QMouseEvent>
#include <iostream>
/**
 * @brief Label subclass for the remote controller key
 *
 */
class Mouse_Label : public QLabel
{
    Q_OBJECT

public:
    //IconEditor(QWidget *parent = 0);
    explicit Mouse_Label(QWidget *parent = 0);
    // bool event(QEvent *e);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *); ///< handles Mouse_Enter_Events
    void leaveEvent(QEvent *);
    ~Mouse_Label();
    void setText(const QString &);
    void clear();
    void show();
    void clicked();
    void released();
    void setText_Clicked(QString);

signals:
    void mouse_leave();
    void mouse_enter();
    void mouse_move();

private:
    QString label_text; ///< text which appears once the label is hovered by mouse
    bool isreleased;
    QString text_clicked; ///< text which appears, when a controller key is pressed
};



#endif // Mouse_LABEL_H
