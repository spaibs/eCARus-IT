#include "special_label.h"
#include <iostream>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <mainwindow.h>
#include <QFrame>

Special_Label::Special_Label(QWidget *parent) :     //Label sub class which provides several mouse events
    QLabel(parent)
{
    label_text = this->text();
    this->setStyleSheet("QLabel { background-color : none; color : red; }");
    this->setFrameShape(QFrame::Box);
    //QLabel::setText("<center><img src=':/new/prefix2/photos/Red_Light.png'/></center>");
    this->setAlignment(Qt::AlignCenter);
}

Special_Label::~Special_Label()
{

}

void Special_Label::clicked()
{}
void Special_Label::released()
{}
void Special_Label::mouseMoveEvent(QMouseEvent *)
{}
void Special_Label::enterEvent(QEvent *)
{
     emit mouse_enter();
}
void Special_Label::leaveEvent(QEvent *)
{
    emit mouse_leave();
}
void Special_Label::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_click();
}
void Special_Label::mouseReleaseEvent(QMouseEvent *ev)
{
    emit mouse_release();
}

