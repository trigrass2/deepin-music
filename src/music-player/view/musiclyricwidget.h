/*
 * Copyright (C) 2016 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QHBoxLayout>

#include <DWidget>

#include <searchmeta.h>
#include "core/playlist.h"

DWIDGET_USE_NAMESPACE

class MUsicLyricWidgetPrivate;
class MUsicLyricWidget : public DWidget
{
    Q_OBJECT
    Q_PROPERTY(QString defaultCover READ defaultCover WRITE setDefaultCover)

public:
    explicit MUsicLyricWidget(QWidget *parent = Q_NULLPTR);
    ~MUsicLyricWidget();

    void updateUI();
    QString defaultCover() const;
    void checkHiddenSearch(QPoint mousePos);

signals:
    void toggleLyricView();
    void requestContextSearch(const QString &context);
    void changeMetaCache(const MetaPtr meta, const DMusic::SearchMeta &search);

public slots:
    void onProgressChanged(qint64 value, qint64 length);
    void onMusicPlayed(PlaylistPtr playlist, const MetaPtr meta);
    void onMusicStop(PlaylistPtr, const MetaPtr);

    void onLyricChanged(const MetaPtr meta,
                        const DMusic::SearchMeta &search,
                        const QByteArray &lyricData);
    void onCoverChanged(const MetaPtr meta,
                        const DMusic::SearchMeta &search,
                        const QByteArray &coverData);

    void onContextSearchFinished(const QString &context,
                                 const QList<DMusic::SearchMeta> &metalist);

    void setDefaultCover(QString defaultCover);
    void onUpdateMetaCodec(const MetaPtr meta);

    void onsearchBt();
    void slotonsearchresult(QString path);

    void slotTheme(int type);

protected:
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QHBoxLayout *m_leftLayout;
    QScopedPointer<MUsicLyricWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), MUsicLyricWidget)
};
