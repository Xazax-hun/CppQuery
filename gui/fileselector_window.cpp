#include "fileselector_window.h"

using namespace CppQuery;

FileSelectorDialog::FileSelectorDialog(const QStringList &list, QWidget *parent)
    : QDialog(parent) {
  resize(800, 600);
  setModal(true);
  QVBoxLayout *layout = new QVBoxLayout(this);

  listWidget = new QListWidget(this);
  selectAllButton = new QPushButton(tr("Select all"), this);
  deselectAllButton = new QPushButton(tr("Deselect all"), this);
  okButton = new QPushButton(tr("Ok"), this);

  QHBoxLayout *buttonlayout = new QHBoxLayout();
  
  buttonlayout->addWidget(selectAllButton);
  buttonlayout->addWidget(deselectAllButton);
  buttonlayout->addWidget(okButton);
  layout->addWidget(listWidget);
  layout->addLayout(buttonlayout);

  listWidget->addItems(list);
  for (int i = 0; i < list.size(); ++i) {
    QListWidgetItem *item = listWidget->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
  }

  connect(selectAllButton, &QPushButton::clicked, this,
          &FileSelectorDialog::selectAll);
  connect(deselectAllButton, &QPushButton::clicked, this,
          &FileSelectorDialog::deselectAll);
  connect(okButton, &QPushButton::clicked, this,
          &FileSelectorDialog::okClicked);
}

void FileSelectorDialog::selectAll() {
  for (int i = 0; i < listWidget->count(); ++i) {
    QListWidgetItem *item = listWidget->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
  }
}

void FileSelectorDialog::deselectAll() {
  for (int i = 0; i < listWidget->count(); ++i) {
    QListWidgetItem *item = listWidget->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
  }
}

void FileSelectorDialog::okClicked() {
  selectedFiles.clear();
  for (int i = 0; i < listWidget->count(); ++i) {
    QListWidgetItem *item = listWidget->item(i);
    if (item->checkState() == Qt::Checked)
      selectedFiles << item->text();
  }
  accept();
}
