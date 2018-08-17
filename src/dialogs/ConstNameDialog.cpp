#include "ConstNameDialog.h"
#include "ui_ConstNameDialog.h"

ConstNameDialog::ConstNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConstNameDialog),
    model(new QStringListModel(this)),
    proxyModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
        
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listView->setModel(proxyModel);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect( ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(on_doubleClicked(const QModelIndex &)) );

    // Ctrl-F to show/hide the filter entry
    QShortcut *searchShortcut = new QShortcut(QKeySequence::Find, this);
    connect(searchShortcut, &QShortcut::activated, ui->quickFilterView, &QuickFilterView::showFilter);
    searchShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    // Esc to clear the filter entry
    QShortcut *clearShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(clearShortcut, &QShortcut::activated, ui->quickFilterView, &QuickFilterView::clearFilter);
    clearShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    connect(ui->quickFilterView, SIGNAL(filterTextChanged(const QString &)),
            proxyModel, SLOT(setFilterWildcard(const QString &)));
}

ConstNameDialog::~ConstNameDialog() {}

void ConstNameDialog::on_buttonBox_accepted()
{
}

void ConstNameDialog::on_buttonBox_rejected()
{
    close();
}

void ConstNameDialog::on_doubleClicked(const QModelIndex &index)
{
    setResult(1);
    close();
}

QString ConstNameDialog::getConstName()
{
    auto selectedList = ui->listView->selectionModel()->selectedIndexes();
    if (selectedList.size() == 0) {
        return nullptr;
    }
    QString ret = selectedList.first().data().toString();
    return ret;
}

void ConstNameDialog::setConstValue(qulonglong constant)
{
    QJsonObject jsonObject = Core()->cmdj("aCj " + QString::number(constant)).object();
    QString str = "0x" + QString("%1").arg(constant, 0, 16);
    QJsonArray names = jsonObject["names"].toArray();
    QStringList list;
    foreach (QJsonValue str, names)
    {
        list << str.toString();
    }
    model->setStringList(list);

}

bool ConstNameDialog::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event -> type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast <QKeyEvent *>(event);

        // Confirm comment by pressing Ctrl/Cmd+Return
        if ((keyEvent -> modifiers() & Qt::ControlModifier) &&
                ((keyEvent -> key() == Qt::Key_Enter) || (keyEvent -> key() == Qt::Key_Return))) {
            this->accept();
            return true;
        }
    }


    return false;
}
