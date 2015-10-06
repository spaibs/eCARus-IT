#include "mouse_label.h"
#include <iostream>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <mainwindow.h>
#include <QFrame>


/*
 * Subclass of QLabel - includes Mouse-Events for the Remote Controller key
 *
 */
Mouse_Label::Mouse_Label(QWidget *parent) :
    QLabel(parent)
{
    label_text = this->text();
    this->setStyleSheet("QLabel { background-color : #0f3357; color:white; }");
    this->setFrameShape(QFrame::Box);
    //QLabel::setText("<center><img src=':/new/prefix2/photos/Red_Light.png'/></center>");
    this->setAlignment(Qt::AlignCenter);
    this->isreleased=false;
}

Mouse_Label::~Mouse_Label()
{

}


void Mouse_Label::setText_Clicked(QString qtext)
{
   text_clicked = qtext;
}
void Mouse_Label::leaveEvent(QEvent *)  //hides the label once the cursor leaves the label
{
    emit mouse_leave();
    clear();
}

void Mouse_Label::mouseMoveEvent(QMouseEvent *)
{
    emit mouse_move();
}

void Mouse_Label::enterEvent(QEvent *)  //set the object visible once the cursor enters the label
{
    show();
    emit mouse_enter();
}

void Mouse_Label::setText(const QString &text)
{
    if(!text.isEmpty())
    label_text = text;
    text_clicked = label_text;
}

void Mouse_Label::clear()   //hides the label
{
    QLabel::setText("");
    this->setStyleSheet("QLabel { background : transparent; color:white; }");
}

void Mouse_Label::show()   //set the object visible
{
    this->setStyleSheet("QLabel { background-color : #0f3357; color:white; }");
    emit mouse_enter();
    QLabel::setText(label_text);
}

void Mouse_Label::clicked()
{
    this->isreleased=false;
    this->setStyleSheet("QLabel { background-color : #0f3357; color : white; }");
    QLabel::setText(text_clicked);
}

void Mouse_Label::released()
{
    if(!this->isreleased)
    {
        this->setStyleSheet("QLabel { background:transparent; color : white; }");
        QLabel::setText("");
    }
    this->isreleased = true;
}






