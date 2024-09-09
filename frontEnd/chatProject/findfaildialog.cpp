#include "findfaildialog.h"

#include "ui_findfaildialog.h"

FindFailDialog::FindFailDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FindFailDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

FindFailDialog::~FindFailDialog() {
    delete ui;
}
