#ifndef DIALOGGETFILENAME_H
#define DIALOGGETFILENAME_H

#include <QDialog>

namespace Ui {
class DialogGetFileName;
}

class DialogGetFileName : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogGetFileName(QWidget *parent = 0);
    ~DialogGetFileName();
    QString fileName;
private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogGetFileName *ui;
};

#endif // DIALOGGETFILENAME_H
