#ifndef CLICKHANDLE_H
#define CLICKHANDLE_H
#include <QObject>
#include <QMouseEvent>
class ClickHandle : public QObject
{
    Q_OBJECT
public:
    explicit ClickHandle(QObject *parent = nullptr);

signals:
    void clicked(int x, int y);

public slots:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CLICKHANDLE_H
