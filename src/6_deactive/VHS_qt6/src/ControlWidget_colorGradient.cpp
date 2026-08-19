#include <ControlWidget_colorGradient.h>
#include <QMimeData>
#include <QCloseEvent>
/*
#include <QWidget>
#include <QTreeView>
#include <QAbstractListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QPainter>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHeaderView>
#include <QStyle>
#include <QImage>

#include <vector>
#include <de_glm.hpp>
*/

// explicit
GradientStopsModel::GradientStopsModel(std::vector<ColorStop> &stops, QObject *parent)
    : QAbstractListModel(parent)
    , m_stops(stops)
{}

int
GradientStopsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_stops.size());
}

int
GradientStopsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant
GradientStopsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return {};
    const ColorStop &s = m_stops[static_cast<size_t>(index.row())];

    if (role == Qt::DisplayRole) {
        return QString("t=%1  rgba(%2,%3,%4,%5)")
                .arg(s.t, 0, 'f', 3)
                .arg(int(s.color.r * 255))
                .arg(int(s.color.g * 255))
                .arg(int(s.color.b * 255))
                .arg(int(s.color.a * 255));
    } else if (role == Qt::DecorationRole) {
        QPixmap pm(32, 16);
        pm.fill(QColor::fromRgbF(s.color.r, s.color.g, s.color.b, s.color.a));
        return pm;
    }
    return {};
}

bool
GradientStopsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return false;

    ColorStop &s = m_stops[static_cast<size_t>(index.row())];
    if (role == Qt::UserRole) {
        // expected: QVariantMap { "t": double, "color": QColor }
        QVariantMap m = value.toMap();
        if (m.contains("t")) {
            s.t = float(m["t"].toDouble());
        }
        if (m.contains("color")) {
            QColor c = m["color"].value<QColor>();
            s.color = glm::vec4(c.redF(), c.greenF(), c.blueF(), c.alphaF());
        }
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::DecorationRole});
        return true;
    }
    return false;
}

Qt::ItemFlags
GradientStopsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    if (!index.isValid())
        f |= Qt::ItemIsDropEnabled;
    else
        f |= Qt::ItemIsDropEnabled;
    return f;
}

QStringList
GradientStopsModel::mimeTypes() const
{
    return { "application/x-gradient-stop-index" };
}

QMimeData *
GradientStopsModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty())
        return nullptr;
    QMimeData *mime = new QMimeData;
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << indexes.first().row();
    mime->setData("application/x-gradient-stop-index", ba);
    return mime;
}

bool
GradientStopsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int, const QModelIndex &)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("application/x-gradient-stop-index"))
        return false;

    QByteArray ba = data->data("application/x-gradient-stop-index");
    QDataStream ds(&ba, QIODevice::ReadOnly);
    int srcRow;
    ds >> srcRow;
    if (srcRow < 0 || srcRow >= rowCount())
        return false;

    // drop position: row is the destination index (before row)
    if (row < 0 || row > rowCount())
        row = rowCount();

    if (row == srcRow || row == srcRow + 1)
        return false;

    beginMoveRows(QModelIndex(), srcRow, srcRow, QModelIndex(), (row > srcRow) ? row : row);
    ColorStop cs = m_stops[static_cast<size_t>(srcRow)];
    m_stops.erase(m_stops.begin() + srcRow);
    if (row > srcRow)
        --row;
    m_stops.insert(m_stops.begin() + row, cs);
    endMoveRows();

    return true;
}

Qt::DropActions
GradientStopsModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

// bool
// GradientStopsModel::insertRow(int row, const QModelIndex &parent)
// {
//     return insertRows(row, 1, parent);
// }

bool
GradientStopsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || count <= 0)
        return false;
    if (row < 0 || row > rowCount())
        row = rowCount();

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_stops.insert(m_stops.begin() + row,
                       ColorStop{ glm::vec4(1.f, 1.f, 1.f, 1.f), 0.5f });
    }
    endInsertRows();
    return true;
}

// bool
// GradientStopsModel::removeRow(int row, const QModelIndex &parent)
// {
//     return removeRows(row, 1, parent);
// }

bool
GradientStopsModel::removeRows(int row, int count, const QModelIndex &parent )
{
    if (parent.isValid() || count <= 0)
        return false;
    if (row < 0 || row + count > rowCount())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    m_stops.erase(m_stops.begin() + row, m_stops.begin() + row + count);
    endRemoveRows();
    return true;
}

const std::vector<ColorStop> &
GradientStopsModel::stops() const { return m_stops; }


// ================================================================

// explicit
GradientPreview::GradientPreview(std::vector<ColorStop> &stops, QWidget *parent)
    : QWidget(parent)
    , m_stops(stops)
{
    setMinimumSize(32, 32);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GradientPreview::updateGradient()
{
    update();
}

void GradientPreview::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    if (m_stops.empty()) {
        return;
    }

    // ensure sorted by t
    std::vector<ColorStop> s = m_stops;
    std::sort(s.begin(), s.end(), [](const ColorStop &a, const ColorStop &b) {
        return a.t < b.t;
    });

    QImage img(size(), QImage::Format_RGBA8888);
    QPainter ip(&img);
    for (int x = 0; x < img.width(); ++x) {
        float u = img.width() > 1 ? float(x) / float(img.width() - 1) : 0.f;

        // find segment
        const ColorStop *s0 = &s.front();
        const ColorStop *s1 = &s.back();
        for (size_t i = 0; i + 1 < s.size(); ++i) {
            if (u >= s[i].t && u <= s[i+1].t) {
                s0 = &s[i];
                s1 = &s[i+1];
                break;
            }
        }

        float span = (s1->t - s0->t);
        float t = span != 0.f ? (u - s0->t) / span : 0.f;
        if (t < 0.f) t = 0.f;
        if (t > 1.f) t = 1.f;

        glm::vec4 c(
            s0->color.r + (s1->color.r - s0->color.r) * t,
            s0->color.g + (s1->color.g - s0->color.g) * t,
            s0->color.b + (s1->color.b - s0->color.b) * t,
            s0->color.a + (s1->color.a - s0->color.a) * t
        );

        QColor qc = QColor::fromRgbF(c.r, c.g, c.b, c.a);
        ip.setPen(qc);
        ip.drawLine(x, 0, x, img.height());
    }
    ip.end();

    p.drawImage(rect(), img);
}


// ===============================================================
ControlWidget_ColorGradient::ControlWidget_ColorGradient(QWidget *parent)
    : HideOnCloseWidget(parent)
    , m_model(nullptr)
    , m_view(nullptr)
    , m_preview(nullptr)
    , m_spinT(nullptr)
    , m_btnColor(nullptr)
    , m_btnApply(nullptr)
    , m_btnAdd(nullptr)
    , m_btnRemove(nullptr)
    , m_btnUp(nullptr)
    , m_btnDown(nullptr)
{
    // Make it small and approximately quadratic
    setWindowTitle("Gradient Editor");
    setMinimumSize(100, 100);

    // Data
    m_stops = {
        { glm::vec4(0.f, 0.f, 0.f, 1.f), 0.0f },
        { glm::vec4(1.f, 1.f, 1.f, 1.f), 1.0f },
    };

    m_model = new GradientStopsModel(m_stops, this);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Top row: New / Load / Save as tiny toolbuttons with icons


    QToolButton *btnNew = new QToolButton(this);
    QToolButton *btnLoad = new QToolButton(this);
    QToolButton *btnSave = new QToolButton(this);

    btnNew->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    btnLoad->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    btnSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    btnNew->setToolTip("New Gradient");
    btnLoad->setToolTip("Load Gradient");
    btnSave->setToolTip("Save Gradient");

    btnNew->setAutoRaise(true);
    btnLoad->setAutoRaise(true);
    btnSave->setAutoRaise(true);

    auto *fileLayout = new QHBoxLayout;
    fileLayout->setSpacing(2);
    fileLayout->addWidget(btnNew);
    fileLayout->addWidget(btnLoad);
    fileLayout->addWidget(btnSave);
    fileLayout->addStretch(1);

    mainLayout->addLayout(fileLayout);

    // Center: TreeView on left, preview on right
    auto *centerLayout = new QHBoxLayout;
    centerLayout->setSpacing(4);

    m_view = new QTreeView(this);
    m_view->setModel(m_model);
    m_view->setRootIsDecorated(false);
    m_view->setHeaderHidden(true);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDropIndicatorShown(true);
    m_view->setDragDropMode(QAbstractItemView::InternalMove);
    m_view->header()->setStretchLastSection(true);
    m_view->setMinimumWidth(80);
    m_view->setMinimumHeight(40);

    centerLayout->addWidget(m_view, 1);

    m_preview = new GradientPreview(m_stops, this);
    centerLayout->addWidget(m_preview, 1);

    mainLayout->addLayout(centerLayout, 1);

    // Bottom: stop editing and add/delete/reorder
    auto *bottomLayout = new QVBoxLayout;
    bottomLayout->setSpacing(4);

    // Row: t + color + OK
    auto *editLayout = new QHBoxLayout;
    editLayout->setSpacing(4);

    m_spinT = new QDoubleSpinBox(this);
    m_spinT->setRange(0.0, 1.0);
    m_spinT->setSingleStep(0.01);
    m_spinT->setDecimals(3);
    m_spinT->setToolTip("Stop position t");

    m_btnColor = new QPushButton("Color", this);
    m_btnApply = new QPushButton("OK", this);

    editLayout->addWidget(m_spinT);
    editLayout->addWidget(m_btnColor);
    editLayout->addWidget(m_btnApply);

    bottomLayout->addLayout(editLayout);

    // Row: add / delete / up / down
    auto *opsLayout = new QHBoxLayout;
    opsLayout->setSpacing(4);

    m_btnAdd = new QPushButton("+", this);
    m_btnRemove = new QPushButton("-", this);
    m_btnUp = new QPushButton("↑", this);
    m_btnDown = new QPushButton("↓", this);

    m_btnAdd->setFixedWidth(28);
    m_btnRemove->setFixedWidth(28);
    m_btnUp->setFixedWidth(28);
    m_btnDown->setFixedWidth(28);

    opsLayout->addWidget(m_btnAdd);
    opsLayout->addWidget(m_btnRemove);
    opsLayout->addWidget(m_btnUp);
    opsLayout->addWidget(m_btnDown);
    opsLayout->addStretch(1);

    bottomLayout->addLayout(opsLayout);

    mainLayout->addLayout(bottomLayout);

    // Connections
    connect(m_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ControlWidget_ColorGradient::onCurrentChanged);

    connect(m_btnColor, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onChooseColor);
    connect(m_btnApply, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onApplyEdit);

    connect(m_btnAdd, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onAddStop);
    connect(m_btnRemove, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onRemoveStop);
    connect(m_btnUp, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onMoveUp);
    connect(m_btnDown, &QPushButton::clicked, this, &ControlWidget_ColorGradient::onMoveDown);

    connect(btnNew, &QToolButton::clicked, this, &ControlWidget_ColorGradient::onNew);
    connect(btnLoad, &QToolButton::clicked, this, &ControlWidget_ColorGradient::onLoad);
    connect(btnSave, &QToolButton::clicked, this, &ControlWidget_ColorGradient::onSave);

    // whenever model changes, repaint preview
    connect(m_model, &QAbstractItemModel::dataChanged, m_preview, &GradientPreview::updateGradient);
    connect(m_model, &QAbstractItemModel::rowsInserted, m_preview, &GradientPreview::updateGradient);
    connect(m_model, &QAbstractItemModel::rowsRemoved, m_preview, &GradientPreview::updateGradient);
    connect(m_model, &QAbstractItemModel::modelReset, m_preview, &GradientPreview::updateGradient);

    if (m_model->rowCount() > 0)
        m_view->setCurrentIndex(m_model->index(0, 0));
}

const std::vector<ColorStop> &
ControlWidget_ColorGradient::stops() const
{
    return m_stops;
}

void
ControlWidget_ColorGradient::onCurrentChanged(const QModelIndex &current, const QModelIndex &)
{
    if (!current.isValid())
        return;
    int row = current.row();
    if (row < 0 || row >= m_model->rowCount())
        return;

    const ColorStop &s = m_stops[static_cast<size_t>(row)];
    m_spinT->setValue(s.t);
    m_currentColor = QColor::fromRgbF(s.color.r, s.color.g, s.color.b, s.color.a);
    updateColorButton();
}

void
ControlWidget_ColorGradient::onChooseColor()
{
    QColor c = QColorDialog::getColor(m_currentColor.isValid() ? m_currentColor : Qt::white, this, "Choose Color");
    if (c.isValid()) {
        m_currentColor = c;
        updateColorButton();
    }
}

void
ControlWidget_ColorGradient::onApplyEdit()
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid())
        return;

    QVariantMap m;
    m["t"] = m_spinT->value();
    m["color"] = m_currentColor;

    m_model->setData(idx, m, Qt::UserRole);
    m_preview->updateGradient();
}

void
ControlWidget_ColorGradient::onAddStop()
{
    QModelIndex idx = m_view->currentIndex();
    int row = idx.isValid() ? idx.row() + 1 : m_model->rowCount();
    m_model->insertRow(row);
    QModelIndex newIdx = m_model->index(row, 0);
    m_view->setCurrentIndex(newIdx);
    m_preview->updateGradient();
}

void
ControlWidget_ColorGradient::onRemoveStop()
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid())
        return;
    if (m_model->rowCount() <= 1)
        return; // keep at least one
    int row = idx.row();
    m_model->removeRow(row);
    int newRow = std::min(row, m_model->rowCount() - 1);
    if (newRow >= 0)
        m_view->setCurrentIndex(m_model->index(newRow, 0));
    m_preview->updateGradient();
}

void
ControlWidget_ColorGradient::onMoveUp()
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid())
        return;
    int row = idx.row();
    if (row <= 0)
        return;
    moveRow(row, row - 1);
}

void
ControlWidget_ColorGradient::onMoveDown()
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid())
        return;
    int row = idx.row();
    if (row >= m_model->rowCount() - 1)
        return;
    moveRow(row, row + 1);
}

void
ControlWidget_ColorGradient::onNew()
{
    std::vector<ColorStop> stops(2);
    stops.push_back({ glm::vec4(0.f, 0.f, 0.f, 1.f), 0.f });
    stops.push_back({ glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f });
    m_model->resetStops(stops);

    m_preview->updateGradient();
    if (m_model->rowCount() > 0)
        m_view->setCurrentIndex(m_model->index(0, 0));
}

void
ControlWidget_ColorGradient::onLoad()
{
    QString fn = QFileDialog::getOpenFileName(this, "Load Gradient", QString(), "Gradient (*.json)");
    if (fn.isEmpty())
        return;

    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly))
        return;

    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
        return;

    QJsonArray arr = doc.array();
    std::vector<ColorStop> tmp;
    tmp.reserve(arr.size());
    for (auto v : arr) {
        QJsonObject o = v.toObject();
        ColorStop s;
        s.t = float(o["t"].toDouble());
        QJsonArray c = o["color"].toArray();
        if (c.size() == 4) {
            s.color = glm::vec4(
                float(c[0].toDouble()),
                float(c[1].toDouble()),
                float(c[2].toDouble()),
                float(c[3].toDouble())
            );
        } else {
            s.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
        }
        tmp.push_back(s);
    }

    m_model->resetStops(tmp);

    m_preview->updateGradient();
    if (m_model->rowCount() > 0)
        m_view->setCurrentIndex(m_model->index(0, 0));
}

void
ControlWidget_ColorGradient::onSave()
{
    QString fn = QFileDialog::getSaveFileName(this, "Save Gradient", QString(), "Gradient (*.json)");
    if (fn.isEmpty())
        return;

    QJsonArray arr;
    for (const ColorStop &s : m_stops) {
        QJsonObject o;
        o["t"] = double(s.t);
        QJsonArray c;
        c.append(double(s.color.r));
        c.append(double(s.color.g));
        c.append(double(s.color.b));
        c.append(double(s.color.a));
        o["color"] = c;
        arr.append(o);
    }
    QJsonDocument doc(arr);

    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
}

void
ControlWidget_ColorGradient::moveRow(int from, int to)
{
    if (from == to)
        return;
    if (from < 0 || from >= m_model->rowCount())
        return;
    if (to < 0 || to >= m_model->rowCount())
        return;

    ColorStop cs = m_stops[static_cast<size_t>(from)];
    m_stops.erase(m_stops.begin() + from);
    m_stops.insert(m_stops.begin() + to, cs);

    m_model->resetStops(m_stops);
    m_view->setCurrentIndex(m_model->index(to, 0));

    m_preview->updateGradient();
    if (m_model->rowCount() > 0)
        m_view->setCurrentIndex(m_model->index(0, 0));
}

void
ControlWidget_ColorGradient::updateColorButton()
{
    if (!m_currentColor.isValid())
        return;
    QPalette pal = m_btnColor->palette();
    pal.setColor(QPalette::Button, m_currentColor);
    m_btnColor->setAutoFillBackground(true);
    m_btnColor->setPalette(pal);
    m_btnColor->update();
}

/*
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ControlWidget_ColorGradient w;
    w.show();

    return app.exec();
}
*/

// #include "main.moc"
