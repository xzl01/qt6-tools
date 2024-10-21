// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QACTIVEXPLUGINOBJECT_H
#define QACTIVEXPLUGINOBJECT_H

#include <QtCore/qpointer.h>
#include <QtCore/qmap.h>
#include <QtWidgets/qwidget.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_NAMESPACE

class QAxWidget;

/* QDesignerAxWidget aggregates QAxWidget to keep it out of the event loop while applying
 * properties directly.
 * Thus, it is possible to set property values in designer that are out of range
 * for the control, which might cause it to throw exceptions.
 *
 * QDesignerAxWidget is the base class following the internal naming
 * conventions that makes the control property visible to the introspection interface.
 *
 * The trick to aggregate a QAxWidget is to overwrite the metaObject() function
 * generated by moc to return the QMetaObject of QAxWidget. This is what QDesignerAxPluginWidget does. */

class QDesignerAxWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString control READ control WRITE setControl RESET resetControl DESIGNABLE true)
    Q_DISABLE_COPY_MOVE(QDesignerAxWidget)

protected:
    explicit QDesignerAxWidget(QWidget *parent);

public:
    virtual ~QDesignerAxWidget();

    bool loadControl(const QString &clsid);

    void resetControl();
    void setControl(const QString &clsid);
    QString control() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool loaded() { return (m_axobject != 0); }

    static QPixmap widgetIcon();

    enum { DrawIndicator = 0x1, DrawFrame = 0x2, DrawControl = 0x4 };

    unsigned drawFlags() const { return m_drawFlags; }
    void setDrawFlags(unsigned f)  { m_drawFlags = f; }

protected:
    void paintEvent(QPaintEvent *event) override;
    QAxWidget *axobject() const { return m_axobject; }

private:
    const QSize m_defaultSize { 80, 70 };
    unsigned m_drawFlags = DrawIndicator | DrawFrame | DrawControl;
    QAxWidget *m_axobject = nullptr;
    QPixmap m_axImage;
};

class QDesignerAxPluginWidget : public QDesignerAxWidget
{
   // No Q_OBJECT here! - meta functionality is overridden
public:
    explicit QDesignerAxPluginWidget(QWidget *parent);
    virtual ~QDesignerAxPluginWidget();

    const QMetaObject *metaObject() const override;
    int qt_metacall(QMetaObject::Call, int, void **) override;

private:
    QMap<int, bool> m_propValues;
};

template <> inline QDesignerAxWidget *qobject_cast<QDesignerAxWidget*>(QObject *o)
{
    if (!o)
        return 0;

    // Unloaded state
    if (strcmp(o->metaObject()->className(), "QDesignerAxWidget") == 0)
        return static_cast<QDesignerAxPluginWidget*>(o);

    // Loaded state with fake meta object
    if (strcmp(o->metaObject()->className(), "QAxWidget") == 0)
        return static_cast<QDesignerAxPluginWidget*>(o);

    return 0;
}

QT_END_NAMESPACE

#endif // ACTIVEQT_EXTRAINFO_H
